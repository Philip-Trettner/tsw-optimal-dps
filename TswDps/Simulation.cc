#include "Simulation.hh"

#include <chrono>
#include <iomanip>
#include <iostream>

#include "CombatLog.hh"

#include "Effects.hh"
#include "Passives.hh"
#include "Signets.hh"

#include "aion/Action.hh"

static float mix(float min, float max, float a)
{
    return min + (max - min) * a;
}
static float saturate(float val)
{
    if (val < 0.f)
        return 0.f;
    if (val > 1.f)
        return 1.f;
    return val;
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
    rotation = DefaultRotation::create();
    gear.loadEmptyDpsGear(Gear::TalismanQuality::QL11);
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
    ACTION();

    // AD
    animaDeviationEffect.name = "Anima Deviation";
    animaDeviationScaling = SkillTable::scaling("Anima Deviation");

    // register effects
    registerEffects();

    // reset skill linkage
    for (auto i = 0; i < (int)EffectSlot::Count; ++i)
        effectSkillIndex[i] = -1;

    // assemble gear per skill
    for (auto skillIdx = 0; skillIdx < SKILL_CNT; ++skillIdx)
    {
        auto const& skill = skills.skills[skillIdx];

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
            skillWeaponIdx[skillIdx] = -1;
        }
        else if (skill.weapon == gear.leftWeapon || skill.weapon == gear.rightWeapon)
        {
            auto const& wp = skill.weapon == gear.leftWeapon ? gear.pieces[Gear::WeaponLeft] : gear.pieces[Gear::WeaponRight];
            s = s + wp.stats;
            passives.push_back(wp.signet.passive);
            skillWeaponIdx[skillIdx] = skill.weapon == gear.leftWeapon ? 0 : 1;
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
            if (!skills.augments[a].affectEverything)
                continue; // individual augments are added on each hit

            s = s + skills.augments[a].bonusStats;
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

            if (p.effect != EffectSlot::Count)
                effectSkillIndex[(int)p.effect] = skillIdx;
        }

        // kickback
        if (gear.kickback.passivetype == PassiveType::Kickback)
            passives.push_back(gear.kickback);

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
        skillPenCritPenalty[skillIdx] = 1.0f;
        if (skill.hits > 0)
            skillPenCritPenalty[skillIdx] = skill.multiHitPenalty();

        skillStats[skillIdx] = s;
        procStats[skillIdx] = ps;
        skillTriggers[skillIdx] = triggers;
        procTriggers[skillIdx] = procTrigger;
    }

    // init vulnerabilities
    vulnDmg[(int)DmgType::None] = 0.f;
    vulnDmg[(int)DmgType::Melee] = 25 / 100.f;
    vulnDmg[(int)DmgType::Magic] = 20 / 100.f;
    vulnDmg[(int)DmgType::Ranged] = 20 / 100.f;

    // fin.
    initialized = true;
}

void Simulation::simulate(int totalTimeIn60th)
{
    // init on demand
    if (!initialized)
        init();

    ACTION();

    std::uniform_real_distribution<float> dice(0.0f, 1.0f);
    assert(rotation && "no rotation set");

    // reset sim
    currHitID = 0;
    currSkillID = 0;
    currEffectCnt = 0;
    random.seed((uint32_t)std::chrono::system_clock::now().time_since_epoch().count());
    rotation->reset();
    currentTime = 0;
    kickbackTime = 0;
    currentWeapon = -1;
    if (buffAt >= INF_TIME)
        dabsTime = INF_TIME;
    else
        dabsTime = buffAt * 60;
    dabsCnt = 0;
    for (auto i = 0; i < SKILL_CNT; ++i)
        skillCDs[i] = 0;
    for (auto i = 0; i < (int)EffectSlot::Count; ++i)
    {
        effectTime[i] = 0;
        effectLastTick[i] = -INF_TIME;
        effectStacks[i] = 0;
        effectHitID[i] = -1;
        effectSkillID[i] = -1;
        currEffectIdx[i] = -1;
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

        // trigger certain augments BEFORE CD
        if (skills.augments[idx].effect != EffectSlot::Count && skills.augments[idx].applyBeforeCD
            && !isOnCooldown(skills.augments[idx].effect))
            procEffect(procStat, skills.augments[idx].effect, -1);

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

        // trigger augments
        if (skills.augments[idx].effect != EffectSlot::Count && !skills.augments[idx].applyBeforeCD
            && !isOnCooldown(skills.augments[idx].effect))
            procEffect(procStat, skills.augments[idx].effect, -1);

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
            for (auto i = 0; i < currEffectCnt; ++i)
            {
                auto slot = (int)currEffects[i];
                assert(effectStacks[slot] > 0);
                if (effects[slot].makeConsumerFree
                    && effects[slot].affects(skill.dmgtype, skill.skilltype, skill.subtype, skill.weapon))
                {
                    consumeResources = false; // e.g. anima charge
                    break;
                }
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
        // special hit on 0 res
        if (skill.dmgScaling0 > 0 && weaponResources[currentWeapon] == 0)
            scaling = skill.dmgScaling0;
        // chance to do more dmg (timber)
        if (skill.chanceForScaleInc > 0)
        {
            if (lowVarianceMode)
                scaling *= 1 + skill.chanceForScaleInc * skill.scaleIncPerc;
            else if (dice(random) < skill.chanceForScaleInc)
                scaling *= 1 + skill.scaleIncPerc;
        }
        skillLastScaling[idx] = scaling;

        // non-channeling builders
        if (!skill.channeling && skill.skilltype == SkillType::Builder)
            addResource(skill.buildPrimaryOnly);

        // hits
        auto hits = skill.hits;

        // anima deviation
        if (skill.animaDeviation)
        {
            bool adCrit, adPen, adGlance, adBlock, adEvade;
            auto adStat = baseStat; // copy
            applyEffects(adStat, DmgType::None, SkillType::None, SubType::None, Weapon::None);
            adStat.update(enemyInfo);
            adStat.finalCritChance = 0.0f; // BUG: AD cannot crit currently
            rawHit(adStat, animaDeviationScaling, 1.0f, DmgType::None, SkillType::None, SubType::None, Weapon::None,
                   &adCrit, &adPen, &adGlance, &adBlock, &adEvade, nullptr, &animaDeviationEffect, -1);
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
            skillLastScaling[idx] = actualScaling;

            // updated stats
            auto stats = baseStat;
            stats.additiveDamage += skill.baseDmgIncPerHit * hitIdx;

            // actual full hit
            fullHit(stats, procStat, actualScaling, penCritPenalty, hitIdx == 0, hitIdx == hits - 1, &skill, nullptr, currentSkill);
        }

        // extra hits
        // hits are instant, using only scaling
        auto extraHits = skill.extraHitPerResource * resourcesConsumed;
        if (extraHits > 0)
        {
            auto stats = baseStat;
            applyEffects(stats, skill.dmgtype, skill.skilltype, skill.subtype, skill.weapon);
            stats = stats + skills.augments[idx].bonusStats;
            stats.update(enemyInfo);

            // Multihit penalty assumed to 0.7
            bool isCrit, isPen, isGlance, isBlock, isEvade;
            for (auto hitIdx = 0; hitIdx < extraHits; ++hitIdx)
                rawHit(stats, scaling, 0.7, skill.dmgtype, skill.skilltype, skill.subtype, skill.weapon, &isCrit,
                       &isPen, &isGlance, &isBlock, &isEvade, &skill, nullptr, idx);
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

void Simulation::analyzeIndividualContribution(
    int fightTime, int maxTime, std::map<std::string, double>& relDmg, StatLog* slog, std::map<int, int>* dmgDistri, int optimizationTime)
{
    auto savMode = lowVarianceMode;
    auto savLog = log;
    log = nullptr;
    lowVarianceMode = true;

    std::cout << "Build Analysis: ";
    std::cout.flush();

    StatLog slog2;
    if (!slog)
        slog = &slog2;
    slog->clear();
    log = slog;
    init();
    resetStats();
    while (totalTimeAccum < maxTime)
        simulate(fightTime);
    auto startDPS = totalDPS();
    auto savTotalDmg = totalDmg;
    auto savTotalHits = totalHits;
    auto savTotalAccum = totalTimeAccum;
    log = nullptr;

    std::cout << "(" << startDPS << " DPS)" << std::endl << std::endl;

    {
        std::cout << "Variance Analysis     ";
        auto minDPS = startDPS;
        auto maxDPS = startDPS;
        auto Edps = startDPS;
        auto Edps2 = startDPS * startDPS;
        int n = 1;
        for (int i = 0; i < 20; ++i)
        {
            std::cout.flush();

            resetStats();
            while (totalTimeAccum < maxTime)
                simulate(fightTime);
            auto dps = totalDPS();

            if (dps < minDPS)
                minDPS = dps;
            if (dps > maxDPS)
                maxDPS = dps;

            Edps += dps;
            Edps2 += dps * dps;
            ++n;

            std::cout << ".";
        }

        auto avg = Edps / n;
        auto dev = sqrt(Edps2 / n - avg * avg);

        auto variance = dev / avg * 3;// (maxDPS - minDPS) / maxDPS;
        std::cout << " " << (variance * 100) << "%" << std::endl;
        relDmg["Variance Analysis"] = variance;
    }

    {
        std::cout << "Variance Optimization ";
        auto minDPS = startDPS;
        auto maxDPS = startDPS;
        auto Edps = startDPS;
        auto Edps2 = startDPS * startDPS;
        int n = 1;
        for (int i = 0; i < 20; ++i)
        {
            std::cout.flush();

            resetStats();
            while (totalTimeAccum < optimizationTime)
                simulate(fightTime);
            auto dps = totalDPS();

            if (dps < minDPS)
                minDPS = dps;
            if (dps > maxDPS)
                maxDPS = dps;

            Edps += dps;
            Edps2 += dps * dps;
            ++n;

            std::cout << ".";
        }

        auto avg = Edps / n;
        auto dev = sqrt(Edps2 / n - avg * avg);

        auto variance = dev / avg * 3;// (maxDPS - minDPS) / maxDPS;
        std::cout << " " << (variance * 100) << "%" << std::endl;
        relDmg["Variance Optimization"] = variance;
        std::cout << std::endl;
    }

    std::cout << "[Damage Distribution]" << std::endl;
    std::map<int, int> dmg2cnt;
    int dmgMaxCnt = 0;
    int dmgSamples = 0;
    for (int i = 0; i < 10; ++i)
    {
        lowVarianceMode = false; // No low variance!
        auto accum = 0;
        while (accum < maxTime)
        {
            accum += fightTime;
            resetStats();
            simulate(fightTime);
            auto dps = totalDPS();

            const int step = 250;
            auto cdps = (int)dps + step / 2;
            cdps -= cdps % step;
            dmg2cnt[cdps]++;
            ++dmgSamples;
            dmgMaxCnt = std::max(dmgMaxCnt, dmg2cnt[cdps]);
        }
        lowVarianceMode = true;
    }
    for (auto const& kvp : dmg2cnt)
    {
        std::cout.width(5);
        std::cout << kvp.first << " DPS: ";
        int length = kvp.second * 50 / dmgMaxCnt;
        std::cout << std::string(length, '=') << std::endl;
    }
    std::cout << "Samples: " << dmgSamples << std::endl;
    std::cout << std::endl;
    if (dmgDistri)
        *dmgDistri = dmg2cnt;

    std::cout << "[Damage Breakdown]" << std::endl;
    totalDmg = savTotalDmg;
    totalHits = savTotalHits;
    totalTimeAccum = savTotalAccum;
    slog->dump(this);
    std::cout << std::endl;

    std::cout << "[Individual Contributions]" << std::endl;
    std::cout << "  Passives:" << std::endl;
    for (auto i = 0u; i < skills.passives.size(); ++i)
    {
        auto passive = skills.passives[i];
        if (passive.name.empty())
            continue;
        skills.passives[i] = Passive();

        init();
        resetStats();
        while (totalTimeAccum < maxTime)
            simulate(fightTime);
        auto dps = totalDPS();

        std::cout << "   + ";
        std::cout.width(5);
        std::cout << std::right << std::fixed << std::setprecision(2) << startDPS * 100. / dps - 100. << "% from '"
                  << passive.name << "'" << std::endl;
        relDmg[passive.name] = startDPS / dps;

        skills.passives[i] = passive;
    }

    std::cout << "  Augments:" << std::endl;
    for (auto i = 0; i < SKILL_CNT; ++i)
    {
        auto aug = skills.augments[i];
        if (aug.name.empty())
            continue;
        skills.augments[i] = Augment();

        init();
        resetStats();
        while (totalTimeAccum < maxTime)
            simulate(fightTime);
        auto dps = totalDPS();

        std::cout << "   + ";
        std::cout.width(5);
        std::cout << std::right << std::fixed << std::setprecision(2) << startDPS * 100. / dps - 100. << "% from '"
                  << aug.name << "'" << std::endl;
        relDmg[aug.name] = startDPS / dps;

        skills.augments[i] = aug;
    }

    std::cout << "  Skills:" << std::endl;
    for (auto i = 0; i < SKILL_CNT; ++i)
    {
        auto skill = skills.skills[i];
        if (skill.name.empty())
            continue;
        if (skill.skilltype == SkillType::Builder)
            continue; // no changing builder
        skills.skills[i] = Skill();

        init();
        resetStats();
        while (totalTimeAccum < maxTime)
            simulate(fightTime);
        auto dps = totalDPS();

        std::cout << "   + ";
        std::cout.width(5);
        std::cout << std::right << std::fixed << std::setprecision(2) << startDPS * 100. / dps - 100. << "% from '"
                  << skill.name << "'" << std::endl;
        relDmg[skill.name] = startDPS / dps;

        skills.skills[i] = skill;
    }

    if (gear.pieces[Gear::MajorLeft].signet.passive.effect == EffectSlot::ConeyIslandBand)
    {
        std::cout << "  Non-Coney: " << std::endl;

        auto piece = gear.pieces[Gear::MajorLeft];
        gear.setFingerQL11(Gear::TalismanQuality::QL11);

        init();
        resetStats();
        while (totalTimeAccum < maxTime)
            simulate(fightTime);
        auto dps = totalDPS();

        std::cout << "   + ";
        std::cout.width(5);
        std::cout << std::right << std::fixed << std::setprecision(2) << dps * 100. / startDPS - 100.
                  << "% for QL11 + Violence" << std::endl;
        relDmg["Finger QL11"] = dps / startDPS;

        gear.pieces[Gear::MajorLeft] = piece;
    }

    std::cout << "  Neck: " << std::endl;
    // QL11 + violence
    if (gear.pieces[Gear::MajorMid].signet.name() != "Violence")
    {
        auto piece = gear.pieces[Gear::MajorMid];
        gear.setNeckQL11(Gear::TalismanQuality::QL11);

        init();
        resetStats();
        while (totalTimeAccum < maxTime)
            simulate(fightTime);
        auto dps = totalDPS();

        std::cout << "   + ";
        std::cout.width(5);
        std::cout << std::right << std::fixed << std::setprecision(2) << dps * 100. / startDPS - 100.
                  << "% for QL11 + Violence" << std::endl;
        relDmg["Neck QL11"] = dps / startDPS;

        gear.pieces[Gear::MajorMid] = piece;
    }
    // WC
    if (gear.pieces[Gear::MajorMid].signet.passive.effect != EffectSlot::MothersWrathStacks)
    {
        auto piece = gear.pieces[Gear::MajorMid];
        gear.setNeckWoodcutters();

        init();
        resetStats();
        while (totalTimeAccum < maxTime)
            simulate(fightTime);
        auto dps = totalDPS();

        std::cout << "   + ";
        std::cout.width(5);
        std::cout << std::right << std::fixed << std::setprecision(2) << dps * 100. / startDPS - 100.
                  << "% for Woodcutters" << std::endl;
        relDmg["Neck WC"] = dps / startDPS;

        gear.pieces[Gear::MajorMid] = piece;
    }
    // Egon
    if (gear.pieces[Gear::MajorMid].signet.passive.effect != EffectSlot::EgonPendant)
    {
        auto piece = gear.pieces[Gear::MajorMid];
        gear.setNeckEgon(Gear::TalismanQuality::QL10_9);

        init();
        resetStats();
        while (totalTimeAccum < maxTime)
            simulate(fightTime);
        auto dps = totalDPS();

        std::cout << "   + ";
        std::cout.width(5);
        std::cout << std::right << std::fixed << std::setprecision(2) << dps * 100. / startDPS - 100.
                  << "% for Amulet of Yuggoth" << std::endl;
        relDmg["Neck Egon"] = dps / startDPS;

        gear.pieces[Gear::MajorMid] = piece;
    }

    std::cout << "  Signets: " << std::endl;
    for (auto i = Gear::Head; i <= Gear::WeaponRight; ++i)
    {
        auto piece = gear.pieces[i];
        auto allPieces = gear.pieces;
        if (piece.signet.name().empty())
            continue;
        gear.pieces[i].signet = Signets::empty();

        if (i == Gear::Head || i == Gear::WeaponLeft || i == Gear::WeaponRight)
        {
            if (gear.pieces[Gear::Head].signet.name() == piece.signet.name())
                gear.pieces[Gear::Head].signet = Signets::empty();
            if (gear.pieces[Gear::WeaponLeft].signet.name() == piece.signet.name())
                gear.pieces[Gear::WeaponLeft].signet = Signets::empty();
            if (gear.pieces[Gear::WeaponRight].signet.name() == piece.signet.name())
                gear.pieces[Gear::WeaponRight].signet = Signets::empty();
        }

        init();
        resetStats();
        while (totalTimeAccum < maxTime)
            simulate(fightTime);
        auto dps = totalDPS();

        std::cout << "   + ";
        std::cout.width(5);
        std::cout << std::right << std::fixed << std::setprecision(2) << startDPS * 100. / dps - 100. << "% from '"
                  << piece.signet.name() << "' on " << gear.pieceName(i) << std::endl;
        relDmg[piece.signet.name()] = startDPS / dps;

        gear.pieces = allPieces;
    }
    std::cout << "  Traditional Signet Distribution: " << std::endl;
    // "Traditional signet distribution"
    {
        auto allPieces = gear.pieces;
        gear.pieces[Gear::Head].signet = Signets::HeadWeapon::Laceration();
        gear.pieces[Gear::WeaponLeft].signet = Signets::HeadWeapon::Abuse();
        gear.pieces[Gear::WeaponRight].signet = Signets::HeadWeapon::Aggression();

        init();
        resetStats();
        while (totalTimeAccum < maxTime)
            simulate(fightTime);
        auto dps = totalDPS();

        std::cout << "   - ";
        std::cout.width(5);
        std::cout << std::right << std::fixed << std::setprecision(2) << startDPS * 100. / dps - 100. << "% from '"
                  << "Laceration-Abuse-Aggression'" << std::endl;
        relDmg["Laceration-Abuse-Aggression"] = dps / startDPS;

        gear.pieces = allPieces;
    }
    {
        auto allPieces = gear.pieces;
        gear.pieces[Gear::Head].signet = Signets::HeadWeapon::Laceration();
        gear.pieces[Gear::WeaponLeft].signet = Signets::HeadWeapon::Aggression();
        gear.pieces[Gear::WeaponRight].signet = Signets::HeadWeapon::Abuse();

        init();
        resetStats();
        while (totalTimeAccum < maxTime)
            simulate(fightTime);
        auto dps = totalDPS();

        std::cout << "   - ";
        std::cout.width(5);
        std::cout << std::right << std::fixed << std::setprecision(2) << startDPS * 100. / dps - 100. << "% from '"
                  << "Laceration-Aggression-Abuse'" << std::endl;
        relDmg["Laceration-Aggression-Abuse"] = dps / startDPS;

        gear.pieces = allPieces;
    }
    // Stimulant
    if (gear.stimulant != EffectSlot::Count)
    {
        std::cout << "  Stimulant: " << std::endl;
        auto stim = gear.stimulant;
        gear.stimulant = EffectSlot::Count;

        init();
        resetStats();
        while (totalTimeAccum < maxTime)
            simulate(fightTime);
        auto dps = totalDPS();

        std::cout << "   + ";
        std::cout.width(5);
        std::cout << std::right << std::fixed << std::setprecision(2) << startDPS * 100. / dps - 100. << "% from "
                  << to_string(stim) << std::endl;
        relDmg["Stimulant"] = startDPS / dps;

        gear.stimulant = stim;
    }
    // Kickback
    if (gear.kickback.passivetype == PassiveType::Kickback)
    {
        std::cout << "  Kickback: " << std::endl;
        auto kb = gear.kickback;
        gear.kickback = Passives::empty();

        init();
        resetStats();
        while (totalTimeAccum < maxTime)
            simulate(fightTime);
        auto dps = totalDPS();

        std::cout << "   + ";
        std::cout.width(5);
        std::cout << std::right << std::fixed << std::setprecision(2) << startDPS * 100. / dps - 100. << "% from "
                  << kb.name << std::endl;
        relDmg["Kickback"] = startDPS / dps;

        gear.kickback = kb;
    }
    // Potion
    if (potionStats != Stats())
    {
        std::cout << "  Potion: " << std::endl;
        auto pot = potionStats;
        potionStats = Stats();

        init();
        resetStats();
        while (totalTimeAccum < maxTime)
            simulate(fightTime);
        auto dps = totalDPS();

        std::cout << "   + ";
        std::cout.width(5);
        std::cout << std::right << std::fixed << std::setprecision(2) << startDPS * 100. / dps - 100. << "% from "
                  << shortStatDump(pot) << " Potion" << std::endl;
        relDmg["Potion"] = startDPS / dps;

        potionStats = pot;
    }

    std::cout << std::endl;
    std::cout << "[Stat Impact]" << std::endl;
    auto savPot = potionStats;
    for (auto r : {Rating::Crit, Rating::CritPower, Rating::Pen, Rating::Hit})
    {
        std::cout << "  ";
        std::cout.width(9);
        std::cout << std::right << to_string(r) << ": {";
        bool first = true;
        for (auto o : {-200, -150, -100, -50, 50, 100, 150, 200})
        {
            if (first)
                first = false;
            else
                std::cout << ", ";
            std::cout.flush();

            Stats offset;
            offset.set(r, o);
            potionStats = savPot + offset;

            init();
            resetStats();
            while (totalTimeAccum < maxTime)
                simulate(fightTime);
            auto dps = totalDPS();

            std::cout << (startDPS < dps ? "+" : "-") << std::fixed << std::setprecision(2)
                      << (startDPS > dps ? startDPS * 100. / dps - 100. : dps * 100. / startDPS - 100.) << "%";

            relDmg["Stat " + to_string(r) + " " + to_string(o)] = dps / startDPS;
        }
        std::cout << "} for {-200, -150, -100, -50, +50, +150, +100, +200}" << std::endl;
    }
    potionStats = savPot;

    log = savLog;
    lowVarianceMode = savMode;
}

jsonxx::Object Simulation::fightToJson() const
{
    jsonxx::Object o;
    o << "Buffs At" << buffAt;
    o << "DABS CD" << toTimeStr(dabsCDIn60th);
    o << "DABS Variance" << toTimeStr(dabsVarianceIn60th);
    o << "Enemy Block" << enemyInfo.stats.blockRating;
    o << "Enemy Defense" << enemyInfo.stats.defenceRating;
    o << "Pen Power" << enemyInfo.penPower;
    o << "Base Vulnerability" << enemyInfo.baseVulnerability;
    o << "All Vulnerabilities" << enemyInfo.allVulnerabilities;
    o << "Low Variance Mode" << lowVarianceMode;
    return o;
}

void Simulation::fightFromJson(const jsonxx::Object& o)
{
    using namespace jsonxx;

    buffAt = (int)o.get<Number>("Buffs At", buffAt);
    dabsCDIn60th = ticksFromJsonObj(o, "DABS CD", dabsCDIn60th);
    dabsVarianceIn60th = ticksFromJsonObj(o, "DABS Variance", dabsVarianceIn60th);
    enemyInfo.stats.blockRating = (int)o.get<Number>("Enemy Block", enemyInfo.stats.blockRating);
    enemyInfo.stats.defenceRating = (int)o.get<Number>("Enemy Defense", enemyInfo.stats.defenceRating);
    enemyInfo.penPower = o.get<Number>("Pen Power", enemyInfo.penPower);
    enemyInfo.baseVulnerability = o.get<Number>("Base Vulnerability", enemyInfo.baseVulnerability);
    enemyInfo.allVulnerabilities = o.get<Boolean>("All Vulnerabilities", enemyInfo.allVulnerabilities);
    lowVarianceMode = o.get<Boolean>("Low Variance Mode", lowVarianceMode);
}

bool Simulation::isOnCooldown(EffectSlot slot) const
{
    return currentTime - effectLastTick[(int)slot] < effects[(int)slot].cooldownIn60th;
}

bool Simulation::isActive(EffectSlot slot) const
{
    return effectStacks[(int)slot] > 0;
}

string Simulation::effectName(EffectSlot slot) const
{
    return effects[(int)slot].name;
}

Effect const& Simulation::effectFor(EffectSlot slot) const
{
    return effects[(int)slot];
}

void Simulation::fullHit(const Stats& baseStats,
                         Stats const& procStat,
                         float dmgScaling,
                         float penCritPenalty,
                         bool startOfAbility,
                         bool endOfAbility,
                         Skill const* srcSkill,
                         Effect const* srcEffect,
                         int skillIndex)
{
    // ACTION(); 850ns
    ++currHitID; // increase hit id

    auto weapon = srcSkill ? srcSkill->weapon : Weapon::None;
    if (!srcSkill && skillIndex >= 0)
        weapon = skills.skills[skillIndex].weapon; // full hits from skill passives
    auto dmgtype = srcSkill ? srcSkill->dmgtype : srcEffect->dmgtype;
    auto skilltype = srcSkill ? srcSkill->skilltype : SkillType::PassiveFullHit;
    auto subtype = srcSkill ? srcSkill->subtype : SubType::None;
    auto stats = baseStats; // copy
    applyEffects(stats, dmgtype, skilltype, subtype, weapon);
    if (skillIndex >= 0 && !skills.augments[skillIndex].affectEverything)
        stats = stats + skills.augments[skillIndex].bonusStats;
    stats.update(enemyInfo);

    bool isCrit, isPen, isGlance, isBlock, isEvade;
    rawHit(stats, dmgScaling, penCritPenalty, dmgtype, skilltype, subtype, weapon, &isCrit, &isPen, &isGlance, &isBlock,
           &isEvade, srcSkill, srcEffect, skillIndex);

    // special hit effects
    for (auto i = 0; i < currEffectCnt; ++i)
    {
        auto slot = (int)currEffects[i];
        auto const& effect = effects[slot];

        assert(effectStacks[slot] > 0);

        // reset on pen
        if (isPen && effect.resetOnPen)
            resetEffect((EffectSlot)slot);

        // reset on glance
        if (isGlance && effect.resetOnGlance)
            resetEffect((EffectSlot)slot);

        // proc on skill hit
        if (srcSkill && effect.procOn == ProcOn::SkillHit && !isOnCooldown(effect.slot)
            && effect.affects(dmgtype, skilltype, subtype, weapon))
        {
            procEffectDmg(procStat, effect, dmgScaling);
            effectLastTick[slot] = currentTime; // CD
        }
    }

    // effects trigger AFTER the hit
    for (auto const& passive : srcSkill ? skillTriggers[currentSkill] : procTriggers[currentSkill])
    {
        auto slot = (size_t)passive.effect;
        auto const& effect = effects[slot];
        assert(effect.slot < EffectSlot::Count && "effect not registered");

        // check trigger
        switch (passive.trigger)
        {
        case Trigger::Hit: // on hit
            break;
        case Trigger::Crit: // on crit
            if (!isCrit)
                continue;
            break;
        case Trigger::Pen: // on pen
            if (!isPen)
                continue;
            break;
        case Trigger::CritPen: // on crit-pen
            if (!(isCrit && isPen))
                continue;
            break;
        default:
            continue; // not triggered by (special) hit
        }

        // once per ability can only proc at start (for now)
        if (effect.oncePerAbility && !startOfAbility)
            continue;

        // on CD
        if (isOnCooldown(passive.effect))
            continue;

        // kickback on non-kb time
        if (passive.passivetype == PassiveType::Kickback && kickbackTime <= 0)
            continue;

        // blocked by pen
        if (isPen && effect.resetOnPen)
            continue;

        // blocked by glance
        if (isGlance && effect.resetOnGlance)
            continue;

        procEffect(procStat, passive, dmgScaling);
    }

    // consumed after hit
    for (auto i = 0; i < currEffectCnt; ++i)
    {
        auto slot = (int)currEffects[i];
        auto const& effect = effects[slot];

        assert(effectStacks[slot] > 0);

        // cannot trigger from the same hit it was (re)gained
        if (effectHitID[slot] == currHitID)
            continue;

        // optionally: cannot conume on same ability
        if (effect.cannotConsumeSameAbility && effectSkillID[slot] == currSkillID)
            continue;

        // not affected
        if (!effect.affects(dmgtype, skilltype, subtype, weapon))
            continue;

        // lose one stack after each hit (or at end of ability)
        if (effect.consumedAfterHit || (effect.consumedAfterAbility && endOfAbility))
        {
            --effectStacks[slot];
            if (log)
                log->logEffectEnd(this, currentTime, (EffectSlot)slot);

            // refresh time
            if (effectStacks[slot] > 0)
            {
                assert(effect.timeIn60th > 0);
                effectTime[slot] = effect.timeIn60th;
            }
            else // or reset it
            {
                effectTime[slot] = 0;
                deactivate((EffectSlot)slot);
                // no CD reduction here?
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
    // ACTION(); <= 100ns, too expensive

    // DO NOT TEST FOR EFFECT CD HERE
    // effect cd is tested earlier

    auto slot = (size_t)effectSlot;
    auto const& effect = effects[slot];
    assert(effect.slot < EffectSlot::Count && "effect not registered");

    // blocked by other effect
    if (effect.blockedSlot < EffectSlot::Count && (isOnCooldown(effect.blockedSlot) || isActive(effect.blockedSlot)))
        return;

    // reduced cooldowns
    if (effect.reducesCooldown > 0)
    {
        auto cdMult = 1.0 - effect.reducesCooldown;

        // for skills
        for (auto& s : skillCDs)
            s *= cdMult;

        // for effects
        for (auto i = 0; i < (int)EffectSlot::Count; ++i)
        {
            if (!effects[i].affectedByCooldownReduction)
                continue;

            auto cd = effects[i].cooldownIn60th - (currentTime - effectLastTick[i]);
            cd *= cdMult;
            effectLastTick[i] = currentTime - effects[i].cooldownIn60th + cd;
        }

        // for dabs
        if (dabsTime != INF_TIME)
            dabsTime *= cdMult;
    }

    // actually procced
    assert(effects[slot].slot < EffectSlot::Count && "effect not registerd");
    if (!effect.noInitialCooldown)
        effectLastTick[slot] = currentTime;
    if (effectStacks[slot] < effect.maxStacks)
    {
        effectHitID[slot] = currHitID;
        effectSkillID[slot] = currSkillID;
    }
    if (effect.timeIn60th > 0)
    {
        assert(effectStacks[slot] >= 0);

        // "activate" effect
        if (effectStacks[slot] == 0)
        {
            assert(currEffectCnt < (int)EffectSlot::Count);
            assert(currEffectIdx[slot] == -1);
            currEffects[currEffectCnt] = effectSlot;
            currEffectIdx[slot] = currEffectCnt;
            // std::cout << currentTime << ": activate " << to_string((EffectSlot)slot) << " (slot " << currEffectCnt <<
            // ")" << std::endl;
            ++currEffectCnt;
        }

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
            for (auto i = 0; i < effect.triggerOnMaxStacksCnt; ++i)
                procEffect(procStats, effect.triggerOnMaxStacks, originalHitScaling);
        }

        // reset on max stacks
        if (effect.resetOnMax && effectStacks[slot] == effect.maxStacks)
            resetEffect(effect.slot);
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

    // group heal on gain
    if (effect.triggersGroupHealOnGain)
        procHeal(procStats, effectSlot, enemyInfo.groupMembers);
}

void Simulation::procEffectDmg(Stats const& procStats, Effect const& effect, float originalHitScaling)
{
    auto skillIdx = effectSkillIndex[(int)effect.slot];

    if (effect.procDmgFixed > 0)
    {
        if (effect.isFullHit)
            fullHit(procStats, procStats, -effect.procDmgFixed, 1.0f, false, false, nullptr, &effect, skillIdx);
        else
        {
            auto stats = procStats + effect.procBonusStats; // copy
            auto weapon = skillIdx >= 0 ? skills.skills[skillIdx].weapon : Weapon::None;
            applyEffects(stats, effect.dmgtype, SkillType::Proc, SubType::None, weapon);
            if (!effect.affectedByAdditiveDmg)
                stats.additiveDamage = 0.f; // procs don't get additive dmg
            if (skillIdx >= 0 && !skills.augments[skillIdx].affectEverything)
                stats = stats + skills.augments[skillIdx].bonusStats;
            stats.update(enemyInfo);
            bool isCrit, isPen, isGlance, isBlock, isEvade;
            rawHit(stats, -effect.procDmgFixed, 1.0f, effect.dmgtype, SkillType::Proc, SubType::None, Weapon::None,
                   &isCrit, &isPen, &isGlance, &isBlock, &isEvade, nullptr, &effect, effectSkillIndex[(int)effect.slot]);
        }
    }

    if (effect.procDmgScaling > 0)
    {
        if (effect.isFullHit)
            fullHit(procStats, procStats, effect.procDmgScaling, 1.0f, false, false, nullptr, &effect, skillIdx);
        else
        {
            auto stats = procStats + effect.procBonusStats; // copy
            auto weapon = skillIdx >= 0 ? skills.skills[skillIdx].weapon : Weapon::None;
            applyEffects(stats, effect.dmgtype, SkillType::Proc, SubType::None, weapon);
            if (!effect.affectedByAdditiveDmg)
                stats.additiveDamage = 0.f; // procs don't get additive dmg
            if (skillIdx >= 0 && !skills.augments[skillIdx].affectEverything)
                stats = stats + skills.augments[skillIdx].bonusStats;
            stats.update(enemyInfo);
            bool isCrit, isPen, isGlance, isBlock, isEvade;
            rawHit(stats, effect.procDmgScaling, 1.0f, effect.dmgtype, SkillType::Proc, SubType::None, Weapon::None,
                   &isCrit, &isPen, &isGlance, &isBlock, &isEvade, nullptr, &effect, effectSkillIndex[(int)effect.slot]);
        }
    }

    if (effect.procDmgPercentage > 0)
    {
        assert(originalHitScaling > 0 && "proc effect at the end of an ability? Oo");
        if (effect.isFullHit)
            fullHit(procStats, procStats, effect.procDmgPercentage * originalHitScaling, 1.0f, false, false, nullptr,
                    &effect, skillIdx);
        else
        {
            auto stats = procStats + effect.procBonusStats; // copy
            auto weapon = skillIdx >= 0 ? skills.skills[skillIdx].weapon : Weapon::None;
            applyEffects(stats, effect.dmgtype, SkillType::Proc, SubType::None, weapon);
            if (!effect.affectedByAdditiveDmg)
                stats.additiveDamage = 0.f; // procs don't get additive dmg
            if (skillIdx >= 0 && !skills.augments[skillIdx].affectEverything)
                stats = stats + skills.augments[skillIdx].bonusStats;
            stats.update(enemyInfo);
            bool isCrit, isPen, isGlance, isBlock, isEvade;

            rawHit(stats, effect.procDmgPercentage * originalHitScaling, 1.0f, effect.dmgtype, SkillType::Proc,
                   SubType::None, Weapon::None, &isCrit, &isPen, &isGlance, &isBlock, &isEvade, nullptr, &effect,
                   effectSkillIndex[(int)effect.slot]);
        }
    }
}

void Simulation::procHeal(const Stats& procStats, EffectSlot effectSlot, int affected)
{
    std::uniform_real_distribution<float> dice(0.0f, 1.0f);

    auto stats = procStats; // copy
    applyEffects(stats, DmgType::None, SkillType::Proc, SubType::None, Weapon::None);
    stats.update(enemyInfo);

    auto critChance = saturate(stats.finalCritChance);
    auto anyCritChance = 1.0 - pow(1.0 - critChance, affected);

    bool isCrit = dice(random) < critChance;
    bool isAnyCrit = dice(random) < anyCritChance;

    if (log)
        log->logHeal(this, currentTime, effectSlot, isCrit, isAnyCrit, affected);

    // trigger
    for (auto const& p : procTriggers[currentSkill])
    {
        switch (p.trigger)
        {
        case Trigger::CritHealSelf:
            if (!isCrit)
                continue;
            break;
        case Trigger::CritHealAny:
            if (!isAnyCrit)
                continue;
            break;
        default:
            continue;
        }

        // cooldown
        if (isOnCooldown(p.effect))
            continue;

        // actual proc
        procEffect(procStats, p, -1);
    }
}

void Simulation::rawHit(const Stats& actualStats,
                        float dmgScaling,
                        float penCritPenalty,
                        DmgType dmgType,
                        SkillType skillType,
                        SubType subType,
                        Weapon weaponType,
                        bool* isCrit,
                        bool* isPen,
                        bool* isGlance,
                        bool* isBlock,
                        bool* isEvade,
                        const Skill* srcSkill,
                        const Effect* srcEffect,
                        int skillIndex)
{
    // ACTION(); ~170ns

    std::uniform_real_distribution<float> dice(0.0f, 1.0f);

    // crit an pen calculation
    float critChance = saturate(actualStats.finalCritChance * penCritPenalty);
    float penChance = saturate(actualStats.finalPenChance * penCritPenalty);
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
        dmg *= mix(1.0f + critChance * critPower, enemyInfo.glancedMultiplier, glanceChance);
        dmg *= mix(1.0f + penChance * enemyInfo.penPower, enemyInfo.blockedMultiplier, blockChance);
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
        log->logHit(this, currentTime, srcSkill ? srcSkill->name : srcEffect->name, dmg, *isCrit, *isPen, *isGlance,
                    *isBlock, *isEvade, dmgType, skillType, subType, weaponType, actualStats, vulnerability);
}

void Simulation::advanceTime(int timeIn60th)
{
    // ACTION(); 500ns

    const int maxNewProcs = 30;
    EffectSlot newProcs[maxNewProcs];
    EffectSlot dmgProcs[maxNewProcs];
    EffectSlot healProcs[maxNewProcs];

    // process passives
    while (timeIn60th > 0)
    {
        // auto trigger
        for (auto const& p : skillTriggers[currentSkill])
            if (p.trigger == Trigger::Auto && effectStacks[(int)p.effect] == 0)
                procEffect(procStats[currentSkill], p, -1);

        // calc time to next event
        auto delta = timeIn60th;
        for (auto i = 0; i < currEffectCnt; ++i)
        {
            auto slot = (int)currEffects[i];
            assert(effectTime[slot] > 0);
            if (effectTime[slot] < delta)
                delta = effectTime[slot];
        }
        if (dabsTime < delta)
            delta = dabsTime;
        // assert(delta > 0); CAN happend due to mercurials

        // actually advance time
        currentTime += delta;
        totalTimeAccum += delta;
        timeIn60th -= delta;
        kickbackTime -= delta;
        if (dabsTime != INF_TIME)
            dabsTime -= delta;

        // reduce skill CDs
        for (auto& cd : skillCDs)
        {
            cd -= delta;
            if (cd < 0)
                cd = 0;
        }

        // reduce vulnerabilities
        for (auto& t : vulnTime)
        {
            t -= delta;
            if (t < 0)
                t = 0;
        }

        // update running effects
        {
            auto i = 0;
            int newProcCnt = 0;
            int dmgProcCnt = 0;
            int healProcCnt = 0;
            while (i < currEffectCnt)
            {
                auto slot = (int)currEffects[i];

                assert(effectTime[slot] > 0);

                effectTime[slot] -= delta;

                bool deactivated = false;

                // effect ended
                if (effectTime[slot] == 0)
                {
                    // loose a stack
                    effectStacks[slot] -= 1;
                    assert(effectStacks[slot] >= 0 && "negative stacks");

                    // refresh time if stacks left
                    if (effectStacks[slot] > 0)
                        effectTime[slot] = effects[slot].timeIn60th;
                    else
                    {
                        // deactivate
                        deactivated = true;
                        deactivate((EffectSlot)slot);
                    }

                    // log
                    if (log)
                        log->logEffectEnd(this, currentTime, (EffectSlot)slot);

                    // trigger dmg on lose
                    if (effects[slot].procOn == ProcOn::Loss)
                        dmgProcs[dmgProcCnt++] = (EffectSlot)slot;

                    // trigger heal on lose
                    if (effects[slot].triggersPlayerHealOnLost)
                        healProcs[healProcCnt++] = (EffectSlot)slot;

                    // trigger on lose
                    if (effects[slot].triggerOnStackLost != EffectSlot::Count)
                        if (!effects[slot].triggerOnStackLostOnlyLast || effectStacks[slot] == 0)
                            newProcs[newProcCnt++] = effects[slot].triggerOnStackLost;

                    assert(dmgProcCnt < maxNewProcs);
                    assert(newProcCnt < maxNewProcs);
                }

                // inc if stacks left
                if (!deactivated)
                    ++i;
            }

            // dmg proc on stack lost
            for (auto i = 0; i < dmgProcCnt; ++i)
                procEffectDmg(
                    procStats[currentSkill], effects[(int)dmgProcs[i]],
                    effectSkillIndex[(int)dmgProcs[i]] == -1 ? -1 : skillLastScaling[effectSkillIndex[(int)dmgProcs[i]]]);
            // proc on stack lost
            for (auto i = 0; i < newProcCnt; ++i)
                procEffect(procStats[currentSkill], newProcs[i], -1);
            // proc player heal on stack lost
            for (auto i = 0; i < healProcCnt; ++i)
                procHeal(procStats[currentSkill], healProcs[i], 1);
        }

        // check buffs
        if (dabsTime == 0)
        {
            std::uniform_int_distribution<int> cd(dabsCDIn60th, dabsCDIn60th + dabsVarianceIn60th);
            dabsTime = cd(random);
            procEffect(Stats(), EffectSlot::MajorCriticalChance, -1);
            procEffect(Stats(), EffectSlot::MajorPenetrationChance, -1);

            // stims + KBs every 2nd dabs
            if (dabsCnt % 2 == 0)
            {
                kickbackTime = 20 * 60;
                if (gear.stimulant != EffectSlot::Count)
                    procEffect(Stats(), gear.stimulant, -1);
            }

            ++dabsCnt;
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
    // ACTION(); <= 90ns aka too expensive

    // add currently running effects
    for (auto i = 0; i < currEffectCnt; ++i)
    {
        auto slot = (int)currEffects[i];
        assert(effectStacks[slot] >= 1);

        if (!effects[slot].hasBonusStats)
            continue; // no bonus stats
        if (!effects[slot].affects(dmgtype, skilltype, subtype, weapon))
            continue; // not affected

        if (effectStacks[slot] > 1)
            stats = stats + effects[slot].bonusStats * (float)effectStacks[slot];
        else
            stats = stats + effects[slot].bonusStats;
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

    deactivate(slot);
}

void Simulation::deactivate(EffectSlot slot)
{
    auto slotIdx = (size_t)slot;

    assert(effectTime[slotIdx] == 0);
    assert(effectStacks[slotIdx] == 0);

    // "deactivate" effect
    if (currEffectIdx[slotIdx] >= 0)
    {
        // std::cout << currentTime << ": deactivate " << to_string(slot) << " (slot " << currEffectIdx[slotIdx] << ")"
        // << std::endl;
        auto ourIdx = currEffectIdx[slotIdx];
        assert(currEffectCnt > 0);
        // copy last effect to this (also works if this is last)
        currEffects[ourIdx] = currEffects[currEffectCnt - 1];
        // update idx of last effect
        currEffectIdx[(int)currEffects[ourIdx]] = ourIdx;
        // remove our idx
        currEffectIdx[slotIdx] = -1;
        // decrease cnt
        --currEffectCnt;
    }
}

void Simulation::registerEffect(const Effect& e)
{
    assert(e.slot < EffectSlot::Count && "invalid effect");
    assert(effects[(int)e.slot].slot == EffectSlot::Count && "already registered");

    effects[(int)e.slot] = e;
    effects[(int)e.slot].hasBonusStats = effects[(int)e.slot].bonusStats != Stats();
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
    registerEffect(Effects::Generic::MajorHitChance());
    registerEffect(Effects::Generic::CritRating());
    registerEffect(Effects::Generic::CritPowerRating());
    registerEffect(Effects::Generic::PenRating());
    registerEffect(Effects::Generic::HitRating());

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
    registerEffect(Effects::Passive::Ferocity());

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
    registerEffect(Effects::Signet::SubwayTokens());
    registerEffect(Effects::Signet::SubwayTokensCountdown());
    registerEffect(Effects::Signet::Equilibrium());
    registerEffect(Effects::Signet::ConeyIslandBand());

    registerEffect(Effects::Proc::FortunateStrike());
    registerEffect(Effects::Proc::OneInTheChamber());
    registerEffect(Effects::Proc::SuddenReturn());
    registerEffect(Effects::Proc::Thunderstruck());
    registerEffect(Effects::Proc::Gnosis());
    registerEffect(Effects::Proc::LiveWireProc());
    registerEffect(Effects::Proc::LiveWireStack());
    registerEffect(Effects::Proc::Tenderising());
    registerEffect(Effects::Proc::GrandSlam());

    registerEffect(Effects::Dots::FireInTheHole());
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
    registerEffect(Effects::Dots::ThirdDegree());
    registerEffect(Effects::Dots::ThirdDegreeStack());

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
    registerEffect(Effects::SkillPassive::Cannibalise());
    registerEffect(Effects::SkillPassive::AnimaCharge());
    registerEffect(Effects::SkillPassive::LockStockBarrel());
    registerEffect(Effects::SkillPassive::LockStockBarrelGain());
    registerEffect(Effects::SkillPassive::SteelEcho());

    registerEffect(Effects::Augments::Curing());
    registerEffect(Effects::Augments::Inspiring());
    registerEffect(Effects::Augments::Accelerating());
    registerEffect(Effects::Augments::Quickening());
    registerEffect(Effects::Augments::Invulnerable());
    registerEffect(Effects::Augments::Salubrious());
    registerEffect(Effects::Augments::Mercurial());

    registerEffect(Effects::Stimulants::StimAttackPurple());
    registerEffect(Effects::Stimulants::StimCritPurple());
    registerEffect(Effects::Stimulants::StimPenPurple());
    registerEffect(Effects::Stimulants::StimCritBlue());
    registerEffect(Effects::Stimulants::StimPenBlue());

    registerEffect(Effects::Kickbacks::KickbackCritPurple());
    registerEffect(Effects::Kickbacks::KickbackCritPowerPurple());
    registerEffect(Effects::Kickbacks::KickbackPenPurple());
    registerEffect(Effects::Kickbacks::KickbackCritBlue());
    registerEffect(Effects::Kickbacks::KickbackCritPowerBlue());
    registerEffect(Effects::Kickbacks::KickbackPenBlue());
}
