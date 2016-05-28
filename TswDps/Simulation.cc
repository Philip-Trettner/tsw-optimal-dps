#include "Simulation.hh"

#include <chrono>
#include <iomanip>
#include <iostream>

#include "CombatLog.hh"

#include "Effects.hh"
#include "Passives.hh"
#include "Signets.hh"

void Simulation::resetStats()
{
    totalDmg = 0;
    totalHits = 0;
    totalPens = 0;
    totalCrits = 0;
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
            std::cout << "skill " << skill.name << " at " << to_string(skill.weapon) << " vs " << to_string(gear.leftWeapon) << " & "
                      << to_string(gear.rightWeapon) << std::endl;
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
                triggers.push_back(passive);
        }

        // calc multihit penalty
        skillPenCritPenalty[i] = 1.0f;
        if (skill.hits > 0)
            skillPenCritPenalty[i] = skill.multiHitPenalty();

        skillStats[i] = s;
        procStats[i] = ps;
        skillTriggers[i] = triggers;
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

        // consumer check
        int resources = skill.skilltype == SkillType::Consumer ? weaponResources[currentWeapon] : -1;
        if (skill.skilltype == SkillType::Consumer && resources == 0 && skill.fixedConsumerResources == 0)
        {
            assert(skill.weapon != Weapon::Blood && "not implemented");
            std::cout << "[ERROR] trying to consume '" << skill.name << "' on zero resources.";
            assert(0);
        }
        if (skill.skilltype == SkillType::Consumer && resources < skill.fixedConsumerResources)
        {
            assert(skill.weapon != Weapon::Blood && "not implemented");
            std::cout << "[ERROR] trying to consume '" << skill.name << "' on " << resources << " resources.";
            assert(0);
        }

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

			procEffect(procStat, passive, -1);
		}

        // non-channeling builders
        if (!skill.channeling && skill.skilltype == SkillType::Builder)
            addResource(skill.buildPrimaryOnly);

        // precalc scaling
        float scaling = skill.dmgScaling;
		if (skill.dmgScalingLow > 0 && stochasticLowHealth)
			scaling += 0.35f * (skill.dmgScalingLow - skill.dmgScaling);
        float penCritPenalty = skillPenCritPenalty[idx];
        // "consumes all resources"
        if (skill.skilltype == SkillType::Consumer && skill.fixedConsumerResources == 0)
        {
            float a = (resources - 1.f) / (5.f - 1.f);
			auto scaling5 = skill.dmgScaling5;
			if (skill.dmgScalingLow > 0 && stochasticLowHealth)
				scaling5 += 0.35f * (skill.dmgScaling5Low - skill.dmgScaling5);
            scaling += (scaling5 - skill.dmgScaling) * a;
        }
        // chance to do more dmg (timber)
        if (skill.chanceForScaleInc > 0)
        {
            if (lowVarianceMode)
                scaling *= 1 + skill.chanceForScaleInc * skill.scaleIncPerc;
            else if (dice(random) < skill.chanceForScaleInc)
                scaling *= 1 + skill.scaleIncPerc;
        }

        // consumers
		int resourcesConsumed = 0;
        if (skill.skilltype == SkillType::Consumer)
        {
            assert(currentWeapon >= 0 && "aux consumer??");

            auto resBefore = weaponResources[currentWeapon];

            // TODO: BLOOD
            if (skill.fixedConsumerResources > 0)
                weaponResources[currentWeapon] -= skill.fixedConsumerResources;
            else
                weaponResources[currentWeapon] = 0;

			// res consumed
			resourcesConsumed = resBefore - weaponResources[currentWeapon];

            if (log)
                log->logResource(this, currentTime, skill.weapon, -resourcesConsumed);
        }

		// hits
		auto hits = skill.hits + skill.extraHitPerResource * resourcesConsumed;

        // actually do skill
		for (auto hitIdx = 0; hitIdx < hits; ++hitIdx)
		{
			// if channeling, advance time first
			if (skill.channeling)
			{
				advanceTime(skill.timeIn60th / hits);
				remainingTime -= skill.timeIn60th / hits;
			}

			// anima deviation
			if (hitIdx == 0 && skill.animaDeviation)
			{
				bool adCrit, adPen;
				auto adStat = baseStat; // copy
				adStat.finalCritChance = 0.0f; // BUG: AD cannot crit currently
				rawHit(adStat, animaDeviationScaling, 1.0f, DmgType::None, &adCrit, &adPen, nullptr, &animaDeviationEffect);
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

            procEffect(procStat, passive, -1);
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

void Simulation::dumpBriefReport()
{
    std::cout << "Dmg:   " << totalDmg << " over " << totalTimeAccum / 60.f << " sec" << std::endl;
    std::cout << "DPS:   " << totalDPS() << std::endl;
    std::cout << "Hits:  " << totalHits << std::endl;
    std::cout << "Crits: " << totalCrits << " (" << totalCrits * 100.f / totalHits << "%)" << std::endl;
    std::cout << "Pens:  " << totalPens << " (" << totalPens * 100.f / totalHits << "%)" << std::endl;
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

    // WC
    if (gear.pieces[Gear::MajorMid].signet.passive.effect == EffectSlot::MothersWrathStacks)
    {
        auto piece = gear.pieces[Gear::MajorMid];
        gear.pieces[Gear::MajorMid].signet = Signets::Major::Violence();
        gear.pieces[Gear::MajorMid].set(PrimaryStat::Attack, Gear::TalismanQuality::QL11);

        init();
        resetStats();
        simulate(maxTime);
        auto dps = totalDPS();

        std::cout << " + ";
        std::cout.width(4);
        std::cout << std::right << std::fixed << std::setprecision(1) << int(startDPS * 1000 / dps - 1000) / 10.
                  << "% from Woodcutters (vs. purple Violence and QL11)" << std::endl;

        gear.pieces[Gear::MajorMid] = piece;
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

    bool isCrit, isPen;
    rawHit(stats, dmgScaling, penCritPenalty, dmgtype, &isCrit, &isPen, srcSkill, srcEffect);

    // special hit effects
    for (auto i = 0u; i < (int)EffectSlot::Count; ++i)
    {
        auto const& effect = effects[i];

        // reset on pen
        if (isPen && effect.resetOnPen && effectStacks[i] > 0)
            resetEffect((EffectSlot)i);
    }

    // effects trigger AFTER the hit
    for (auto const& passive : skillTriggers[currentSkill])
    {
        auto slot = (size_t)passive.effect;
        auto const& effect = effects[slot];
        //if (effect.slot == EffectSlot::Count)
        //    std::cerr << "Unregistered effect " << effect.name << " for " << passive.name << std::endl;
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

        // wrong weapon type
        if (effect.restrictToWeapon != Weapon::None && weapon != effect.restrictToWeapon)
            continue;

        // wrong skill type
        if (effect.restrictToSkillType != SkillType::None && skilltype != effect.restrictToSkillType)
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
    if (effect.procDmgScaling > 0)
    {
        auto stats = procStats; // copy
        applyEffects(stats, effect.dmgtype, SkillType::Proc, SubType::None, Weapon::None);
		if (!effect.affectedByAdditiveDmg)
			stats.additiveDamage = 0.f; // procs don't get additive dmg
        stats.update(enemyInfo);
        bool isCrit, isPen;
        rawHit(stats, effect.procDmgScaling, 1.0f, effect.dmgtype, &isCrit, &isPen, nullptr, &effect);
    }
    if (effect.procDmgPercentage > 0)
    {
        assert(originalHitScaling > 0 && "proc effect at the end of an ability? Oo");
        auto stats = procStats; // copy
        applyEffects(stats, effect.dmgtype, SkillType::Proc, SubType::None, Weapon::None);
		if (!effect.affectedByAdditiveDmg)
			stats.additiveDamage = 0.f; // procs don't get additive dmg
        stats.update(enemyInfo);
        bool isCrit, isPen;
        rawHit(stats, effect.procDmgPercentage * originalHitScaling, 1.0f, effect.dmgtype, &isCrit, &isPen, nullptr, &effect);
    }

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

void Simulation::rawHit(const Stats& actualStats,
                        float dmgScaling,
                        float penCritPenalty,
                        DmgType dmgType,
                        bool* isCrit,
                        bool* isPen,
                        const Skill* srcSkill,
                        const Effect* srcEffect)
{
    std::uniform_real_distribution<float> dice(0.0f, 1.0f);

    // crit an pen calculation
    float critChance = actualStats.finalCritChance * penCritPenalty;
    float penChance = actualStats.finalPenChance * penCritPenalty;
    float critPower = actualStats.finalCritPower;

    float vulnerability = 1 + enemyInfo.baseVulnerability;
    if (vulnTime[(int)dmgType] > 0)
        vulnerability += vulnDmg[(int)dmgType];

    float dmg = actualStats.finalCombatPower * dmgScaling * actualStats.finalDmgMultiplier * vulnerability;

    *isCrit = dice(random) < critChance;
    *isPen = dice(random) < penChance;

    if (!lowVarianceMode)
    {
        if (*isCrit)
            dmg *= 1.0f + critPower;

        if (*isPen)
            dmg *= 1.0f + enemyInfo.penPower;
    }
    else
    {
        dmg *= 1.0f + fmin(critChance, 1.0f) * critPower;
        dmg *= 1.0f + fmin(penChance, 1.0f) * enemyInfo.penPower;
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

    // log
    if (log)
        log->logHit(this, currentTime, srcSkill ? srcSkill->name : srcEffect->name, dmg, *isCrit, *isPen, actualStats, vulnerability);
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

					// trigger on lose
					if (effects[i].triggerOnStackLost != EffectSlot::Count)
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
        if (skilltype == SkillType::Proc && !effects[i].affectProcs)
            continue; // not all effects affect procs

        if (effects[i].restrictToWeapon != Weapon::None && weapon != effects[i].restrictToWeapon)
            continue; // does not affect this weapon

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
    registerEffect(Effects::Passive::Lethality());
    registerEffect(Effects::Passive::TwistTheKnife());

    registerEffect(Effects::Signet::Abuse());
    registerEffect(Effects::Signet::Aggression());
    registerEffect(Effects::Signet::Laceration());
    registerEffect(Effects::Signet::MothersWrathBuff());
    registerEffect(Effects::Signet::MothersWrathStacks());

    registerEffect(Effects::Proc::FortunateStrike());
    registerEffect(Effects::Proc::OneInTheChamber());
    registerEffect(Effects::Proc::SuddenReturn());
    registerEffect(Effects::Proc::Thunderstruck());
    registerEffect(Effects::Proc::Gnosis());
    registerEffect(Effects::Proc::LiveWireProc());
	registerEffect(Effects::Proc::LiveWireStack());
	registerEffect(Effects::Proc::BombardmentStacks());
	registerEffect(Effects::Proc::Bombardment());
	registerEffect(Effects::Proc::Tenderising());

    registerEffect(Effects::WeaponSkill::Calamity());
    registerEffect(Effects::WeaponSkill::DoubleUp());
    registerEffect(Effects::WeaponSkill::ElementalOverload());
    registerEffect(Effects::WeaponSkill::MomentumStack());
    registerEffect(Effects::WeaponSkill::MomentumBuff());
    registerEffect(Effects::WeaponSkill::LockStockBarrel());
    registerEffect(Effects::WeaponSkill::LockStockBarrelGain());

    registerEffect(Effects::SkillPassive::Reckless());
    registerEffect(Effects::SkillPassive::AmorFati());
    registerEffect(Effects::SkillPassive::FullMomentum());
}
