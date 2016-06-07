#include "Simulation.hh"

#include <chrono>
#include <iomanip>
#include <iostream>

#include "CombatLog.hh"

#include "Effects.hh"
#include "Passives.hh"
#include "Signets.hh"

static float mix(float min, float max, float a)
{
	return min + (max - min) * a;
}

void Simulation::resetStats()
{
    totalDmg = 0;
    totalHits = 0;
    totalPens = 0;
    totalCrits = 0;
	totalBlocks = 0;
	totalGlances = 0;
	totalEvades = 0;
    totalTimeAccum = 0;
}

Simulation::Simulation()
{
}

void Simulation::loadBuild(const Build& b)
{
    skills = b.skills;
    gear = b.gear;
    potionStats = b.potionStats;
    rotation = b.rotation;
}

Build Simulation::build() const
{
    return {skills, gear, rotation, potionStats};
}

void Simulation::init()
{
    // AD
    animaDeviationEffect.name = "Anima Deviation";
    animaDeviationScaling = SkillTable::scaling("Anima Deviation");

    // register effects
    registerEffects();

    // assemble gear per skill
    for (auto i = 0; i < SKILL_CNT; ++i)
    {
        auto const& skill = skills.skills[i];

        if (skill.weapon == Weapon::None)
            continue; // unused slot

        // sanity checks
        if (skill.hits > 0)
            assert(skill.timeIn60th % skill.hits == 0 && "inconsistent hits over time");

        Stats s;
        std::vector<Passive> triggers;
        std::vector<Passive> procTrigger;
        std::vector<Passive> passives;

        // add weapon
        if (skill.weapon == Weapon::Aux)
        {
            s.weaponPower = 457;
            skillWeaponIdx[i] = -1;
        }
        else if (skill.weapon == gear.leftWeapon || skill.weapon == gear.rightWeapon)
        {
            auto const& wp = skill.weapon == gear.leftWeapon ? gear.pieces[Gear::WeaponLeft] : gear.pieces[Gear::WeaponRight];
            s = s + wp.stats;
            passives.push_back(wp.signet.passive);
            skillWeaponIdx[i] = skill.weapon == gear.leftWeapon ? 0 : 1;
        }
        else
        {
            std::cout << "skill " << skill.name << " at " << to_string(skill.weapon) << " vs "
                      << to_string(gear.leftWeapon) << " & " << to_string(gear.rightWeapon) << std::endl;
            assert(0 && "used skill for non-equipped weapon");
        }

        // add potion
        s = s + potionStats;

        // add gear
        for (auto gi = Gear::Head; gi <= Gear::MinorRight; ++gi)
        {
            auto const& p = gear.pieces[gi];

            // add all talisman stats
            s = s + p.stats;

            // add signets
            if (p.signet.slot != SignetSlot::None)
            {
                passives.push_back(p.signet.passive);

                // sanity
                switch (p.slot)
                {
                case Gear::Slot::Head:
                    assert(p.signet.slot == SignetSlot::Head || p.signet.slot == SignetSlot::HeadWeapon);
                    break;
                case Gear::Slot::Major:
                    assert(p.signet.slot == SignetSlot::Major);
                    break;
                case Gear::Slot::Minor:
                    assert(p.signet.slot == SignetSlot::Minor);
                    break;
                case Gear::Slot::Weapon:
                default:
                    assert(0);
                }
            }
        }

        // calculate proc stats in parallel
        // proc stats are
        //  - gear stats
        //  - potions
        //  - "affecting all" augs
        //  - "affecting procs" passives
        Stats ps = s;

        // add aug(s)
        for (auto a = 0; a < SKILL_CNT; ++a)
        {
            if (skills.augments[a].affectEverything || i == a)
                s = s + skills.augments[a].bonusStats;

            if (skills.augments[a].affectEverything)
                ps = ps + skills.augments[a].bonusStats;
        }

        // TODO: add weapon skill passive
        passives.push_back(Passives::Chaos::Calamity());
        passives.push_back(Passives::Pistol::DoubleUp());
        passives.push_back(Passives::Elemental::ElementalOverload());
        passives.push_back(Passives::Hammer::Momentum());

        // skill passive
        {
            auto p = skill.passive;
            p.skillPassive = true;
            p.name = skill.name + " [passive]";
            passives.push_back(p);
        }

        // add passives
        for (auto const& passive : skills.passives)
            passives.push_back(passive);
        for (auto const& passive : passives)
        {
            if (passive.restrictWeapon && passive.weaponType != skill.weapon)
                continue; // passive does not apply to this weapon

            if (passive.restrictType != SkillType::None && passive.restrictType != skill.skilltype)
                continue; // passive does not apply to this skill type

            if (passive.restrictSubType != SubType::None && passive.restrictSubType != skill.subtype)
                continue; // passive does not apply to this sub type

            // add passive stats
            s = s + passive.bonusStats;

            // ... for procs (e.g. 1.5% signets)
            if (passive.affectsProcs())
                ps = ps + passive.bonusStats;

            // extract trigger passives
            if (passive.trigger != Trigger::None)
            {
                triggers.push_back(passive);
                if (!passive.skillPassive)
                    procTrigger.push_back(passive);
            }
        }

        // calc multihit penalty
        skillPenCritPenalty[i] = 1.0f;
        if (skill.hits > 0)
            skillPenCritPenalty[i] = skill.multiHitPenalty();

        skillStats[i] = s;
        procStats[i] = ps;
        skillTriggers[i] = triggers;
        procTriggers[i] = procTrigger;
    }

    // init vulnerabilities
    vulnDmg[(int)DmgType::None] = 0.f;
    vulnDmg[(int)DmgType::Melee] = 25 / 100.f;
    vulnDmg[(int)DmgType::Magic] = 20 / 100.f;
    vulnDmg[(int)DmgType::Ranged] = 20 / 100.f;
}

void Simulation::simulate(int totalTimeIn60th)
{
    std::uniform_real_distribution<float> dice(0.0f, 1.0f);
    assert(rotation && "no rotation set");

    // reset sim
    currHitID = 0;
    currSkillID = 0;
    random.seed((uint32_t)std::chrono::system_clock::now().time_since_epoch().count());
    rotation->reset();
    currentTime = 0;
    dabsTime = buffAt * 60;
    for (auto i = 0; i < SKILL_CNT; ++i)
        skillCDs[i] = 0;
    for (auto i = 0; i < (int)EffectSlot::Count; ++i)
    {
        effectTime[i] = 0;
        effectCD[i] = 0;
        effectStacks[i] = 0;
        effectHitID[i] = -1;
        effectSkillID[i] = -1;
    }
    for (auto i = 0; i < (int)DmgType::Count; ++i)
        vulnTime[i] = enemyInfo.allVulnerabilities ? totalTimeIn60th + 1 : 0;
    weaponResources[0] = startsWithResources(gear.leftWeapon) ? 5 : 0;
    weaponResources[1] = startsWithResources(gear.rightWeapon) ? 5 : 0;
    if (resetStatsAtStart)
        resetStats();

    // sim for given amount of time
    while (currentTime < totalTimeIn60th)
    {
        auto idx = rotation->nextSkill(currentTime, *this);
        assert(idx >= 0 && idx < SKILL_CNT);
        auto const& skill = skills.skills[idx];
        currentSkill = idx;

        // inc skill and hit (for non-hit abilities) id
        ++currHitID;
        ++currSkillID;

        // log skill
        if (log)
            log->logSkill(this, currentTime, idx);

        // wait for CD
        if (skillCDs[idx] > 0)
        {
            if (warnOnWait)
                std::cout << "[WARNING] waiting " << skillCDs[idx] << " ticks for " << skill.name << std::endl;
            advanceTime(skillCDs[idx]);
            assert(skillCDs[idx] == 0);
        }

        auto const& baseStat = skillStats[idx];
        auto const& procStat = procStats[idx];

        auto remainingTime = skill.timeIn60th;

        // 'equip' weapon
        currentWeapon = skillWeaponIdx[idx];

        // apply CD
        skillCDs[idx] = skill.cooldownIn60th;

        // casttime
        advanceTime(skill.casttimeIn60th);
        remainingTime -= skill.casttimeIn60th;

        // effects that trigger BEFORE first hit
        for (auto const& passive : skillTriggers[idx])
        {
            // start activation
            if (passive.trigger != Trigger::StartActivation)
                continue;

            // non-dmg ability
            if (passive.triggerOnDamaging && skill.hits == 0)
                continue;

            procEffect(procStat, passive, -1);
        }

        // non-channeling builders
        if (!skill.channeling && skill.skilltype == SkillType::Builder)
            addResource(skill.buildPrimaryOnly);


        // consumers
        int resourcesConsumed = 0;
        bool procBloodOffering = false;
        bool consumes = skill.skilltype == SkillType::Consumer || skill.consumesAnyways;
        if (consumes)
        {
            assert(currentWeapon >= 0 && "aux consumer??");

            auto resBefore = weaponResources[currentWeapon];

            // check if consumes resources
            bool consumeResources = true;
            for (auto i = 0; i < (int)EffectSlot::Count; ++i)
                if (effectStacks[i] > 0 && effects[i].makeConsumerFree
                    && effects[i].affects(skill.dmgtype, skill.skilltype, skill.subtype, skill.weapon))
                {
                    consumeResources = false; // e.g. anima charge
                    break;
                }

            // if actually requires resources
            if (consumeResources)
            {
                // fixed res consumer
                if (skill.fixedConsumerResources > 0)
                {
                    weaponResources[currentWeapon] -= skill.fixedConsumerResources;
                    if (weaponResources[currentWeapon] < 0)
                    {
                        assert(skill.weapon == Weapon::Blood && "trying to consume below 0 on non-blood");
                        if (effectStacks[(int)EffectSlot::BloodOffering] > 0)
                            assert(false && "trying to consume below 0 while blood offering is active");
                        weaponResources[currentWeapon] = 0;
                        procBloodOffering = true;
                    }
                }
                else // consumes all res
                {
                    assert(weaponResources[currentWeapon] > 0 || skill.consumesAnyways && "trying to consume on 0 res");
                    weaponResources[currentWeapon] = 0;
                }

                // res consumed
                resourcesConsumed = resBefore - weaponResources[currentWeapon];

                if (log)
                    log->logResource(this, currentTime, skill.weapon, -resourcesConsumed);
            }
        }

        // precalc scaling
        float scaling = skill.dmgScaling;
        if (skill.dmgScalingLow > 0 && stochasticLowHealth)
            scaling += 0.35f * (skill.dmgScalingLow - skill.dmgScaling);
        float penCritPenalty = skillPenCritPenalty[idx];
        // "consumes all resources"
        if (consumes && skill.fixedConsumerResources == 0)
        {
            float a = (resourcesConsumed - 1.f) / (5.f - 1.f);
            auto scaling5 = skill.dmgScaling5;
            if (skill.dmgScalingLow > 0 && stochasticLowHealth)
                scaling5 += 0.35f * (skill.dmgScaling5Low - skill.dmgScaling5);
            scaling += (scaling5 - skill.dmgScaling) * a;

            if (resourcesConsumed == 0)
                scaling = 0; // Bullet Balle @0 resources
        }
        // chance to do more dmg (timber)
        if (skill.chanceForScaleInc > 0)
        {
            if (lowVarianceMode)
                scaling *= 1 + skill.chanceForScaleInc * skill.scaleIncPerc;
            else if (dice(random) < skill.chanceForScaleInc)
                scaling *= 1 + skill.scaleIncPerc;
        }

        // hits
        auto hits = skill.hits + skill.extraHitPerResource * resourcesConsumed;

        // anima deviation
        if (skill.animaDeviation)
        {
            bool adCrit, adPen, adGlance, adBlock, adEvade;
            auto adStat = baseStat;        // copy
            adStat.finalCritChance = 0.0f; // BUG: AD cannot crit currently
            rawHit(adStat, animaDeviationScaling, 1.0f, DmgType::None, &adCrit, &adPen, &adGlance, &adBlock, &adEvade, nullptr, &animaDeviationEffect);
        }

        // actually do skill
        for (auto hitIdx = 0; hitIdx < hits; ++hitIdx)
        {
            // if channeling, advance time first
            if (skill.channeling)
            {
                advanceTime(skill.timeIn60th / hits);
                remainingTime -= skill.timeIn60th / hits;
            }

            // special hits
            auto actualScaling = scaling;
            if (hitIdx < skill.specialHitsA)
                actualScaling = skill.dmgScalingA;
            else if (hitIdx < skill.specialHitsA + skill.specialHitsB)
                actualScaling = skill.dmgScalingB;
            else if (hitIdx < skill.specialHitsA + skill.specialHitsB + skill.specialHitsC)
                actualScaling = skill.dmgScalingC;
            actualScaling *= 1 + skill.baseDmgIncPerHit * hitIdx;

            // actual full hit
            fullHit(baseStat, procStat, actualScaling, penCritPenalty, hitIdx == 0, hitIdx == hits - 1, &skill, nullptr);
        }

        // channeling builders
        if (skill.channeling && skill.skilltype == SkillType::Builder)
            addResource(skill.buildPrimaryOnly);

        // effects that trigger AFTER last hit
        for (auto const& passive : skillTriggers[idx])
        {
            // finish activation
            if (passive.trigger != Trigger::FinishActivation)
                continue;

            // non-dmg ability
            if (passive.triggerOnDamaging && hits == 0)
                continue;

            procEffect(procStat, passive, -1);
        }

        // Blood offering
        if (procBloodOffering)
            procEffect(procStat, EffectSlot::BloodOffering, -1);

        // reduce CD effect
        if (skill.reduceWeaponConsumerCD > 0)
            for (auto i = 0; i < SKILL_CNT; ++i)
                if (skills.skills[i].weapon == skill.weapon && skills.skills[i].skilltype == SkillType::Consumer)
                {
                    skillCDs[i] -= skill.reduceWeaponConsumerCD;
                    if (skillCDs[i] < 0)
                        skillCDs[i] = 0;
                }

        // inc skill id
        ++currHitID;

        // advance remaining time
        advanceTime(remainingTime);
    }
}

void Simulation::dumpSkillStats()
{
    for (auto i = 0; i < SKILL_CNT; ++i)
    {
        if (skills.skills[i].weapon == Weapon::None)
            continue; // unused slot

        std::cout << "[" << skills.skills[i].name << "]" << std::endl;
        skillStats[i].update(enemyInfo);
        skillStats[i].dumpDpsStats(false);
        std::cout << "- triggers: [";
        bool first = true;
        for (auto const& p : skillTriggers[i])
        {
            if (first)
                first = false;
            else
                std::cout << ", ";
            std::cout << p.name;
        }
        std::cout << "]" << std::endl;
        std::cout << std::endl;
    }
}

void Simulation::dumpBriefReport() const
{
    std::cout << "Dmg:   " << totalDmg << " over " << totalTimeAccum / 60.f << " sec" << std::endl;
    std::cout << "DPS:   " << totalDPS() << std::endl;
    std::cout << "Hits:  " << totalHits << std::endl;
    std::cout << "Crits: " << totalCrits << " (" << totalCrits * 100.f / totalHits << "%)" << std::endl;
	std::cout << "Pens:  " << totalPens << " (" << totalPens * 100.f / totalHits << "%)" << std::endl;
	std::cout << "Glances:  " << totalGlances << " (" << totalGlances * 100.f / totalHits << "%)" << std::endl;
	std::cout << "Blocks:   " << totalBlocks << " (" << totalBlocks * 100.f / totalHits << "%)" << std::endl;
	std::cout << "Evades:   " << totalEvades << " (" << totalEvades * 100.f / totalHits << "%)" << std::endl;
}

void Simulation::analyzePassiveContribution(int maxTime)
{
    auto savMode = lowVarianceMode;
    auto savLog = log;
    log = nullptr;
    lowVarianceMode = true;

    std::cout << "Passive Analysis:" << std::endl;

    init();
    resetStats();
    simulate(maxTime);
    auto startDPS = totalDPS();

    for (auto i = 0u; i < skills.passives.size(); ++i)
    {
        auto passive = skills.passives[i];
        if (passive.name.empty())
            continue;
        skills.passives[i] = Passive();

        init();
        resetStats();
        simulate(maxTime);
        auto dps = totalDPS();

        std::cout << " + ";
        std::cout.width(4);
        std::cout << std::right << std::fixed << std::setprecision(1) << int(startDPS * 1000 / dps - 1000) / 10.
                  << "% from '" << passive.name << "'" << std::endl;

        skills.passives[i] = passive;
    }

    std::cout << "Neck: " << std::endl;
    // QL11 + violence
    {
        auto piece = gear.pieces[Gear::MajorMid];
        gear.setNeckQL11();

        init();
        resetStats();
        simulate(maxTime);
        auto dps = totalDPS();

        std::cout << " + ";
        std::cout.width(4);
        std::cout << std::right << std::fixed << std::setprecision(1) << int(dps * 1000 / startDPS - 1000) / 10.
                  << "% for QL11 + Violence" << std::endl;

        gear.pieces[Gear::MajorMid] = piece;
    }
    // WC
    {
        auto piece = gear.pieces[Gear::MajorMid];
        gear.setNeckWoodcutters();

        init();
        resetStats();
        simulate(maxTime);
        auto dps = totalDPS();

        std::cout << " + ";
        std::cout.width(4);
        std::cout << std::right << std::fixed << std::setprecision(1) << int(dps * 1000 / startDPS - 1000) / 10.
                  << "% for Woodcutters" << std::endl;

        gear.pieces[Gear::MajorMid] = piece;
    }
    // Egon
    {
        auto piece = gear.pieces[Gear::MajorMid];
        gear.setNeckEgon();

        init();
        resetStats();
        simulate(maxTime);
        auto dps = totalDPS();

        std::cout << " + ";
        std::cout.width(4);
        std::cout << std::right << std::fixed << std::setprecision(1) << int(dps * 1000 / startDPS - 1000) / 10.
                  << "% for Amulet of Yuggoth" << std::endl;

        gear.pieces[Gear::MajorMid] = piece;
    }

	std::cout << "Signets: " << std::endl;
	// Head
	{
		auto piece = gear.pieces[Gear::Head];
		gear.pieces[Gear::Head].signet = Signets::empty();

		init();
		resetStats();
		simulate(maxTime);
		auto dps = totalDPS();

		std::cout << " + ";
		std::cout.width(4);
		std::cout << std::right << std::fixed << std::setprecision(1) << int(startDPS * 1000 / dps - 1000) / 10.
			<< "% from " << piece.signet.name() << " on Head" << std::endl;

		gear.pieces[Gear::Head] = piece;
	}
	// Left
	{
		auto piece = gear.pieces[Gear::WeaponLeft];
		gear.pieces[Gear::WeaponLeft].signet = Signets::empty();

		init();
		resetStats();
		simulate(maxTime);
		auto dps = totalDPS();

		std::cout << " + ";
		std::cout.width(4);
		std::cout << std::right << std::fixed << std::setprecision(1) << int(startDPS * 1000 / dps - 1000) / 10.
			<< "% from " << piece.signet.name() << " on " << to_string(gear.leftWeapon) << std::endl;

		gear.pieces[Gear::WeaponLeft] = piece;
	}
	// Right
	{
		auto piece = gear.pieces[Gear::WeaponRight];
		gear.pieces[Gear::WeaponRight].signet = Signets::empty();

		init();
		resetStats();
		simulate(maxTime);
		auto dps = totalDPS();

		std::cout << " + ";
		std::cout.width(4);
		std::cout << std::right << std::fixed << std::setprecision(1) << int(startDPS * 1000 / dps - 1000) / 10.
			<< "% from " << piece.signet.name() << " on " << to_string(gear.rightWeapon) << std::endl;

		gear.pieces[Gear::WeaponRight] = piece;
	}

    log = savLog;
    lowVarianceMode = savMode;
}

void Simulation::fullHit(const Stats& baseStats,
                         Stats const& procStat,
                         float dmgScaling,
                         float penCritPenalty,
                         bool startOfAbility,
                         bool endOfAbility,
                         Skill const* srcSkill,
                         Effect const* srcEffect)
{
    ++currHitID; // increase hit id

    auto weapon = srcSkill ? srcSkill->weapon : Weapon::None;
    auto dmgtype = srcSkill ? srcSkill->dmgtype : srcEffect->dmgtype;
    auto skilltype = srcSkill ? srcSkill->skilltype : SkillType::PassiveFullHit;
    auto subtype = srcSkill ? srcSkill->subtype : SubType::None;
    auto stats = baseStats; // copy
    applyEffects(stats, dmgtype, skilltype, subtype, weapon);
    stats.update(enemyInfo);

    bool isCrit, isPen, isGlance, isBlock, isEvade;
    rawHit(stats, dmgScaling, penCritPenalty, dmgtype, &isCrit, &isPen, &isGlance, &isBlock, &isEvade, srcSkill, srcEffect);

    // special hit effects
    for (auto i = 0u; i < (int)EffectSlot::Count; ++i)
    {
        auto const& effect = effects[i];

        // reset on pen
        if (isPen && effect.resetOnPen && effectStacks[i] > 0)
            resetEffect((EffectSlot)i);

        // proc on skill hit
        if (srcSkill && effect.procOn == ProcOn::SkillHit && effectStacks[i] > 0 && effect.affects(dmgtype, skilltype, subtype, weapon))
            procEffectDmg(procStat, effect, dmgScaling);
    }

    // effects trigger AFTER the hit
    for (auto const& passive : srcSkill ? skillTriggers[currentSkill] : procTriggers[currentSkill])
    {
        auto slot = (size_t)passive.effect;
        auto const& effect = effects[slot];
        assert(effect.slot < EffectSlot::Count && "effect not registered");

        // finish activation is handled differently
        if (passive.trigger == Trigger::FinishActivation)
            continue;

        // once per ability can only proc at start (for now)
        if (effect.oncePerAbility && !startOfAbility)
            continue;

        // on CD
        if (effectCD[slot] > 0)
            continue;

        // on hit
        // if (false && passive.trigger == Trigger::Hit)
        //     continue;

        // on crit
        if (!isCrit && passive.trigger == Trigger::Crit)
            continue;

        // on pen
        if (!isPen && passive.trigger == Trigger::Pen)
            continue;

        // blocked by pen
        if (isPen && effect.resetOnPen)
            continue;

        // on crit-pen
        if (!(isCrit && isPen) && passive.trigger == Trigger::CritPen)
            continue;

        procEffect(procStat, passive, dmgScaling);
    }

    // consumed after hit
    for (auto i = 0; i < (int)EffectSlot::Count; ++i)
    {
        auto const& effect = effects[i];

        if (effectStacks[i] == 0)
            continue;

        // cannot trigger from the same hit it was (re)gained
        if (effectHitID[i] == currHitID)
            continue;

        // optionally: cannot conume on same ability
        if (effect.cannotConsumeSameAbility && effectSkillID[i] == currSkillID)
            continue;

        // not affected
        if (!effect.affects(dmgtype, skilltype, subtype, weapon))
            continue;

        // lose one stack after each hit (or at end of ability)
        if (effect.consumedAfterHit || (effect.consumedAfterAbility && endOfAbility))
        {
            --effectStacks[i];
            if (log)
                log->logEffectEnd(this, currentTime, (EffectSlot)i);

            // refresh time
            if (effectStacks[i] > 0)
                effectTime[i] = effect.timeIn60th;
            else // or reset it
            {
                effectTime[i] = 0;
                effectCD[i] = 0;
            }

            // gain on consume
            if (effect.gainOnConsume != EffectSlot::Count)
                procEffect(procStat, effect.gainOnConsume, dmgScaling);
        }
    }

    // vulnerability
    if (srcSkill && srcSkill->appliesVulnerability != DmgType::None && startOfAbility)
    {
        int v = (int)srcSkill->appliesVulnerability;
        if (vulnTime[v] < 15 * 60)
            vulnTime[v] = 15 * 60;
    }
}

void Simulation::procEffect(const Stats& procStats, const Passive& passive, float originalHitScaling)
{
    std::uniform_real_distribution<float> dice(0.0f, 1.0f);

    // x% chance to trigger
    if (passive.triggerChance < 1.0f && dice(random) > passive.triggerChance)
        return;

    // blocked by effect ability
    if (passive.abilityBlockedEffect != EffectSlot::Count && currSkillID == effectSkillID[(int)passive.abilityBlockedEffect])
        return;

    // add N stacks
    for (auto i = 0; i < passive.effectStacks; ++i)
        procEffect(procStats, passive.effect, originalHitScaling);
}

void Simulation::procEffect(const Stats& procStats, EffectSlot effectSlot, float originalHitScaling)
{
    auto slot = (size_t)effectSlot;
    auto const& effect = effects[slot];
    assert(effect.slot < EffectSlot::Count && "effect not registered");

    // blocked by other effect
    if (effect.blockedSlot < EffectSlot::Count
        && (effectCD[(int)effect.blockedSlot] > 0 || effectStacks[(int)effect.blockedSlot] > 0))
        return;

    // actually procced
    assert(effects[slot].slot < EffectSlot::Count && "effect not registerd");
    effectCD[slot] = effect.cooldownIn60th;
    if (effectStacks[slot] < effect.maxStacks)
    {
        effectHitID[slot] = currHitID;
        effectSkillID[slot] = currSkillID;
    }
    if (effect.timeIn60th > 0)
    {
        effectTime[slot] = effect.timeIn60th;
        effectStacks[slot] += 1;
        if (effectStacks[slot] > effect.maxStacks)
        {
            effectStacks[slot] = effect.maxStacks;
        }
        else
        {
            // log (only on new stack)
            if (log)
                log->logEffectStart(this, currentTime, effect.slot);
        }

        // trigger on max stacks
        if (effect.triggerOnMaxStacks < EffectSlot::Count && effectStacks[slot] == effect.maxStacks)
        {
            // remove all old stacks
            resetEffect(effect.slot);

            // gain new effect
            procEffect(procStats, effect.triggerOnMaxStacks, originalHitScaling);
        }
    }
    else
        assert(effectTime[slot] == 0);

    // proc dmg
    if (effect.procOn == ProcOn::Gain)
        procEffectDmg(procStats, effect, originalHitScaling);

    // trigger on max res
    if (effect.triggerOnMaxRes != EffectSlot::Count && currentWeapon >= 0 && weaponResources[currentWeapon] == 5)
        procEffect(procStats, effect.triggerOnMaxRes, originalHitScaling);

    // gain resources
    for (auto r = 0; r < effect.gainResources; ++r)
        addResource(true);

    // gain other stacks
    if (effect.gainEffectStacks != EffectSlot::Count)
    {
        if (effect.gainEffectStacksTo > 0)
        {
            auto stacks = effect.gainEffectStacksTo - effectStacks[(int)effect.gainEffectStacks];
            while (stacks-- > 0)
                procEffect(procStats, effect.gainEffectStacks, originalHitScaling);
        }
        else
            assert(0 && "invalid");
    }
}

void Simulation::procEffectDmg(Stats const& procStats, Effect const& effect, float originalHitScaling)
{
    if (effect.procDmgFixed > 0)
    {
        auto stats = procStats; // copy
        applyEffects(stats, effect.dmgtype, SkillType::Proc, SubType::None, Weapon::None);
        if (!effect.affectedByAdditiveDmg)
            stats.additiveDamage = 0.f; // procs don't get additive dmg
        stats.update(enemyInfo);
		bool isCrit, isPen, isGlance, isBlock, isEvade;
        if (effect.isFullHit)
            fullHit(procStats, procStats, -effect.procDmgFixed, 1.0f, false, false, nullptr, &effect);
        else
            rawHit(stats, -effect.procDmgFixed, 1.0f, effect.dmgtype, &isCrit, &isPen, &isGlance, &isBlock, &isEvade, nullptr, &effect);
    }

    if (effect.procDmgScaling > 0)
    {
        auto stats = procStats; // copy
        applyEffects(stats, effect.dmgtype, SkillType::Proc, SubType::None, Weapon::None);
        if (!effect.affectedByAdditiveDmg)
            stats.additiveDamage = 0.f; // procs don't get additive dmg
        stats.update(enemyInfo);
		bool isCrit, isPen, isGlance, isBlock, isEvade;
        if (effect.isFullHit)
            fullHit(procStats, procStats, effect.procDmgScaling, 1.0f, false, false, nullptr, &effect);
        else
            rawHit(stats, effect.procDmgScaling, 1.0f, effect.dmgtype, &isCrit, &isPen, &isGlance, &isBlock, &isEvade, nullptr, &effect);
    }

    if (effect.procDmgPercentage > 0)
    {
        assert(originalHitScaling > 0 && "proc effect at the end of an ability? Oo");
        auto stats = procStats; // copy
        applyEffects(stats, effect.dmgtype, SkillType::Proc, SubType::None, Weapon::None);
        if (!effect.affectedByAdditiveDmg)
            stats.additiveDamage = 0.f; // procs don't get additive dmg
        stats.update(enemyInfo);
		bool isCrit, isPen, isGlance, isBlock, isEvade;
        if (effect.isFullHit)
            fullHit(procStats, procStats, effect.procDmgPercentage * originalHitScaling, 1.0f, false, false, nullptr, &effect);
        else
            rawHit(stats, effect.procDmgPercentage * originalHitScaling, 1.0f, effect.dmgtype, &isCrit, &isPen, &isGlance, &isBlock, &isEvade, nullptr, &effect);
    }
}

void Simulation::rawHit(const Stats& actualStats,
                        float dmgScaling,
                        float penCritPenalty,
                        DmgType dmgType,
                        bool* isCrit,
                        bool* isPen,
						bool* isGlance, 
						bool* isBlock, 
						bool* isEvade,
                        const Skill* srcSkill,
                        const Effect* srcEffect)
{
    std::uniform_real_distribution<float> dice(0.0f, 1.0f);

    // crit an pen calculation
    float critChance = actualStats.finalCritChance * penCritPenalty;
	float penChance = actualStats.finalPenChance * penCritPenalty;
	float glanceChance = actualStats.finalGlanceChance;
	float blockChance = actualStats.finalBlockChance;
	float evadeChance = actualStats.finalEvadeChance;
    float critPower = actualStats.finalCritPower;

    float vulnerability = 1 + enemyInfo.baseVulnerability;
    if (vulnTime[(int)dmgType] > 0)
        vulnerability += vulnDmg[(int)dmgType];

    float dmg = actualStats.finalCombatPower * dmgScaling * actualStats.finalDmgMultiplier;
    if (dmgScaling < 0)
        dmg = -dmgScaling; // fixed dmg
    dmg *= vulnerability;

    *isCrit = false;
	*isPen = false;
	*isGlance = dice(random) < glanceChance;
	*isBlock = dice(random) < blockChance;
	*isEvade = dice(random) < evadeChance;

	if (!*isGlance)
		*isCrit = dice(random) < critChance; // only non-glances can crit

	if (!*isBlock)
		*isPen = dice(random) < penChance; // only non-blocks can pen

    if (!lowVarianceMode)
    {
        if (*isCrit)
            dmg *= 1.0f + critPower;

        if (*isPen)
            dmg *= 1.0f + enemyInfo.penPower;

		if (*isGlance)
			dmg *= enemyInfo.glancedMultiplier;

		if (*isBlock)
			dmg *= enemyInfo.blockedMultiplier;

		if (*isEvade)
			dmg = 0.0f;
    }
    else
    {
		dmg *= 1.0f - evadeChance;
		dmg *= mix(1.0f + fmin(critChance, 1.0f) * critPower, enemyInfo.glancedMultiplier, glanceChance);
		dmg *= mix(1.0f + fmin(penChance, 1.0f) * enemyInfo.penPower, enemyInfo.blockedMultiplier, blockChance);
		// dmg *= 1.0f + glanceChance * (enemyInfo.glancedMultiplier - 1.0f);
		// dmg *= 1.0f + blockChance * (enemyInfo.blockedMultiplier - 1.0f);
        // dmg *= 1.0f + fmin(critChance, 1.0f) * (1 - glanceChance) * critPower;
        // dmg *= 1.0f + fmin(penChance, 1.0f) * (1 - blockChance) * enemyInfo.penPower;
    }

    // 10% base variance in each dir
    if (!lowVarianceMode)
        dmg *= 1.0f + (dice(random) * 2 - 1) * 0.1f;

    // add hit
    totalDmg += dmg;
    totalHits += 1;
    if (*isCrit)
        totalCrits += 1;
	if (*isPen)
		totalPens += 1;
	if (*isGlance)
		totalGlances += 1;
	if (*isBlock)
		totalBlocks += 1;
	if (*isEvade)
		totalEvades += 1;

    // log
    if (log)
        log->logHit(this, currentTime, srcSkill ? srcSkill->name : srcEffect->name, dmg, *isCrit, *isPen, *isGlance, *isBlock, *isEvade, actualStats, vulnerability);
}

void Simulation::advanceTime(int timeIn60th)
{
    // reduce skill CDs
    for (auto& cd : skillCDs)
    {
        cd -= timeIn60th;
        if (cd < 0)
            cd = 0;
    }
    // reduce effect CDs
    for (auto& cd : effectCD)
    {
        cd -= timeIn60th;
        if (cd < 0)
            cd = 0;
    }

    // process passives
    while (timeIn60th > 0)
    {
        // calc time to next event
        auto delta = timeIn60th;
        for (auto i = 0; i < (int)EffectSlot::Count; ++i)
            if (effectTime[i] > 0 && effectTime[i] < delta)
                delta = effectTime[i];
        if (dabsTime < delta)
            delta = dabsTime;

        // actually advance time
        currentTime += delta;
        totalTimeAccum += delta;
        timeIn60th -= delta;
        dabsTime -= delta;

        // reduce vulnerabilities
        for (auto& t : vulnTime)
        {
            t -= delta;
            if (t < 0)
                t = 0;
        }

        // update running effects
        for (auto i = 0; i < (int)EffectSlot::Count; ++i)
            if (effectTime[i] > 0)
            {
                effectTime[i] -= delta;

                // effect ended
                if (effectTime[i] == 0)
                {
                    // loose a stack
                    effectStacks[i] -= 1;

                    // refresh time if stacks left
                    if (effectStacks[i] > 0)
                        effectTime[i] = effects[i].timeIn60th;

                    // log
                    if (log)
                        log->logEffectEnd(this, currentTime, (EffectSlot)i);

                    // trigger dmg on lose
                    if (effects[i].procOn == ProcOn::Loss)
                        procEffectDmg(procStats[currentSkill], effects[i], -1);

                    // trigger on lose
                    if (effects[i].triggerOnStackLost != EffectSlot::Count)
                        if (!effects[i].triggerOnStackLostOnlyLast || effectStacks[i] == 0)
                            procEffect(procStats[currentSkill], effects[i].triggerOnStackLost, -1);
                }
            }

        // check buffs
        if (dabsTime == 0)
        {
            std::uniform_int_distribution<int> cd(dabsCDIn60th, dabsCDIn60th + dabsVarianceIn60th);
            dabsTime = cd(random);
            procEffect(Stats(), EffectSlot::MajorCriticalChance, -1);
            procEffect(Stats(), EffectSlot::MajorPenetrationChance, -1);
        }
    }
}

void Simulation::addResource(bool currentOnly)
{
    if (currentWeapon == -1)
        return; // build on aux?

    // build primary
    if (weaponResources[currentWeapon] < 5)
    {
        ++weaponResources[currentWeapon];

        if (log)
            log->logResource(this, currentTime, currentWeapon == 0 ? gear.leftWeapon : gear.rightWeapon, 1);
    }

    // build secondary
    if (!currentOnly)
        if (weaponResources[1 - currentWeapon] < 5)
        {
            ++weaponResources[1 - currentWeapon];

            if (log)
                log->logResource(this, currentTime, currentWeapon == 1 ? gear.leftWeapon : gear.rightWeapon, 1);
        }
}

void Simulation::applyEffects(Stats& stats, DmgType dmgtype, SkillType skilltype, SubType subtype, Weapon weapon)
{
    // TODO: check if Lethality etc. affect procs

    // add currently running effects
    for (auto i = 0; i < (int)EffectSlot::Count; ++i)
    {
        if (!effects[i].affects(dmgtype, skilltype, subtype, weapon))
            continue; // not affected

        if (effectStacks[i] > 1)
            stats = stats + effects[i].bonusStats * (float)effectStacks[i];
        else if (effectStacks[i] == 1)
            stats = stats + effects[i].bonusStats;
    }
}

void Simulation::resetEffect(EffectSlot slot)
{
    auto idx = (size_t)slot;

    // remove all old stacks
    if (log)
        while (effectStacks[idx] > 0)
        {
            if (log)
                log->logEffectEnd(this, currentTime, slot);
            effectStacks[idx]--;
        }
    effectStacks[idx] = 0;
    effectTime[idx] = 0;
}

void Simulation::registerEffect(const Effect& e)
{
    assert(e.slot < EffectSlot::Count && "invalid effect");
    assert(effects[(int)e.slot].slot == EffectSlot::Count && "already registered");

    effects[(int)e.slot] = e;
}

void Simulation::registerEffects()
{
    if (effectsRegistered)
        return;
    effectsRegistered = true;

    registerEffect(Effects::Generic::MinorPenetrationChance());
    registerEffect(Effects::Generic::MajorPenetrationChance());
    registerEffect(Effects::Generic::MinorCriticalChance());
    registerEffect(Effects::Generic::MajorCriticalChance());

    registerEffect(Effects::Passive::ElementalForceBuff());
    registerEffect(Effects::Passive::ElementalForceStacks());
    registerEffect(Effects::Passive::FatalFlourishBuff());
    registerEffect(Effects::Passive::FatalFlourishStacks());
    registerEffect(Effects::Passive::DoomBuff());
    registerEffect(Effects::Passive::DoomStacks());
    registerEffect(Effects::Passive::Lethality());
    registerEffect(Effects::Passive::TwistTheKnife());
    registerEffect(Effects::Passive::InvasiveMeasures());
    registerEffect(Effects::Passive::InvasiveMeasuresProc());

    registerEffect(Effects::Signet::Abuse());
    registerEffect(Effects::Signet::Aggression());
    registerEffect(Effects::Signet::Laceration());
    registerEffect(Effects::Signet::MothersWrathBuff());
    registerEffect(Effects::Signet::MothersWrathStacks());
    registerEffect(Effects::Signet::EgonPendant());
    registerEffect(Effects::Signet::FuryBuff());
    registerEffect(Effects::Signet::FuryStacks());
    registerEffect(Effects::Signet::Sadism());
    registerEffect(Effects::Signet::Opportunism());

    registerEffect(Effects::Proc::FortunateStrike());
    registerEffect(Effects::Proc::OneInTheChamber());
    registerEffect(Effects::Proc::SuddenReturn());
    registerEffect(Effects::Proc::Thunderstruck());
    registerEffect(Effects::Proc::Gnosis());
    registerEffect(Effects::Proc::LiveWireProc());
    registerEffect(Effects::Proc::LiveWireStack());
    registerEffect(Effects::Proc::Tenderising());
    registerEffect(Effects::Proc::GrandSlam());

    registerEffect(Effects::Dots::Bombardment());
    registerEffect(Effects::Dots::Whiteout());
    registerEffect(Effects::Dots::GoForTheThroat());
    registerEffect(Effects::Dots::EyeOfPandemonium());
    registerEffect(Effects::Dots::PowerLine());
    registerEffect(Effects::Dots::PowerLineDetonation());
    registerEffect(Effects::Dots::FireManifestation());
    registerEffect(Effects::Dots::LightningManifestation());
    registerEffect(Effects::Dots::Plague());
    registerEffect(Effects::Dots::Contaminate());
    registerEffect(Effects::Dots::LeftHandOfDarkness());

    registerEffect(Effects::WeaponSkill::Calamity());
    registerEffect(Effects::WeaponSkill::DoubleUp());
    registerEffect(Effects::WeaponSkill::ElementalOverload());
    registerEffect(Effects::WeaponSkill::MomentumStack());
    registerEffect(Effects::WeaponSkill::MomentumBuff());
    registerEffect(Effects::WeaponSkill::BloodOffering());
    registerEffect(Effects::WeaponSkill::ElementalFury());

    registerEffect(Effects::SkillPassive::Reckless());
    registerEffect(Effects::SkillPassive::AmorFati());
    registerEffect(Effects::SkillPassive::FullMomentum());
    registerEffect(Effects::SkillPassive::TearEmUp());
    registerEffect(Effects::SkillPassive::GunFu());
    registerEffect(Effects::SkillPassive::LockAndLoad());
    registerEffect(Effects::SkillPassive::Cannibalize());
    registerEffect(Effects::SkillPassive::AnimaCharge());
    registerEffect(Effects::SkillPassive::LockStockBarrel());
    registerEffect(Effects::SkillPassive::LockStockBarrelGain());
    registerEffect(Effects::SkillPassive::SteelEcho());
}
