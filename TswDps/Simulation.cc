#include "Simulation.hh"

#include <iostream>
#include <chrono>

#include "CombatLog.hh"

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

void Simulation::init()
{
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
            passives.push_back(wp.signet.effect);
            skillWeaponIdx[i] = skill.weapon == gear.leftWeapon ? 0 : 1;
        }
        else
            assert(0 && "used skill for non-equipped weapon");

        // add aug(s)
        for (auto a = 0; a < SKILL_CNT; ++a)
            if (skills.augments[a].affectEverything || i == a)
                s = s + skills.augments[a].bonusStats;

        // add potion
        s = s + potionStats;

        // add gear
        for (auto i = Gear::Head; i <= Gear::MinorRight; ++i)
        {
            auto const& p = gear.pieces[i];

            // add all talisman stats
            s = s + p.stats;

            // add signets
            if (p.signet.slot != SignetSlot::None)
            {
                passives.push_back(p.signet.effect);

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
                default:
                    assert(0);
                }
            }
        }

        // TODO: add weapon skill passive

        // add passives
        for (auto const& passive : skills.passives)
            passives.push_back(passive);
        for (auto const& passive : passives)
        {
            if (passive.restrictWeapon && passive.weaponType != skill.weapon)
                continue; // passive does not apply to this weapon

            // add passive stats
            s = s + passive.bonusStats;
        }

        // calc multihit penalty
        skillPenCritPenalty[i] = 1.0f;
        if (skill.hits > 0)
            skillPenCritPenalty[i] = skill.multiHitPenalty();

        // extract trigger passives
        for (auto const& passive : passives)
            if (passive.trigger != Trigger::None)
                triggers.push_back(passive);

        skillStats[i] = s;
        skillTriggers[i] = triggers;
    }
}

void Simulation::simulate(int totalTimeIn60th)
{
    assert(rotation && "no rotation set");

    // reset sim
    random.seed((uint32_t)std::chrono::system_clock::now().time_since_epoch().count());
    rotation->reset();
    currentTime = 0;
    for (auto i = 0; i < SKILL_CNT; ++i)
        skillCDs[i] = 0;
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

        // wait for CD
        if (skillCDs[idx] > 0)
        {
            if (warnOnWait)
                std::cout << "[WARNING] waiting " << skillCDs[idx] << " ticks for " << skill.name << std::endl;
            advanceTime(skillCDs[idx]);
            assert(skillCDs[idx] == 0);
        }

        auto const& baseStats = skillStats[idx];

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

        // casttime
        advanceTime(skill.casttimeIn60th);
        remainingTime -= skill.casttimeIn60th;

        // non-channeling builders
        if (!skill.channeling && skill.skilltype == SkillType::Builder)
            addResource(skill.buildPrimaryOnly);

        // precalc scaling
        float scaling = skill.dmgScaling;
        float penCritPenalty = skillPenCritPenalty[idx];
        // "consumes all resources"
        if (skill.skilltype == SkillType::Consumer && skill.fixedConsumerResources == 0)
        {
            float a = (resources - 1.f) / (5.f - 1.f);
            scaling += (skill.dmgScaling5 - skill.dmgScaling) * a;
        }

        // actually do skill
        for (auto hitIdx = 0; hitIdx < skill.hits; ++hitIdx)
        {
            // if channeling, advance time first
            if (skill.channeling)
            {
                advanceTime(skill.timeIn60th / skill.hits);
                remainingTime -= skill.timeIn60th / skill.hits;
            }

            // actual full hit
            fullHit(baseStats, scaling, penCritPenalty, &skill, nullptr);
        }

        // consumers
        if (skill.skilltype == SkillType::Consumer)
        {
            assert(currentWeapon >= 0 && "aux consumer??");
            weaponResources[currentWeapon] = 0;
        }

        // channeling builders
        if (skill.channeling && skill.skilltype == SkillType::Builder)
            addResource(skill.buildPrimaryOnly);

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
    std::cout << "Dmg:  " << totalDmg << " over " << totalTimeAccum / 60.f << " sec" << std::endl;
    std::cout << "DPS:  " << totalDPS() << std::endl;
    std::cout << "Hits: " << totalHits << std::endl;
    std::cout << "Pens: " << totalPens << " (" << totalPens * 100.f / totalHits << "%)"<< std::endl;
    std::cout << "Pens: " << totalCrits << " (" << totalCrits * 100.f / totalHits << "%)"<< std::endl;
}

void Simulation::fullHit(const Stats &baseStats, float dmgScaling, float penCritPenalty, Skill const* srcSkill, Passive const* srcPassive)
{
    std::uniform_real_distribution<float> dice(0.0f, 1.0f);

    auto stats = baseStats;
    // TODO: add currently running effects
    stats.update(enemyInfo);

    // crit an pen calculation
    float critChance = stats.finalCritChance * penCritPenalty;
    float penChance = stats.finalPenChance * penCritPenalty;
    float critPower = stats.finalCritPower;

    float dmg = stats.finalCombatPower * dmgScaling;

    bool isCrit = dice(random) < critChance;
    bool isPen = dice(random) < penChance;

    if (!lowVarianceMode)
    {
        if (isCrit)
            dmg *= 1.0f + critPower;

        if (isPen)
            dmg *= 1.0f + enemyInfo.penPower;
    }
    else
    {
        dmg *= 1.0f + critChance * critPower;
        dmg *= 1.0f + penChance * enemyInfo.penPower;
    }

    // 10% base variance in each dir
    if (!lowVarianceMode)
        dmg *= 1.0f + (dice(random) * 2 - 1) * 0.1f;

    // add hit
    totalDmg += dmg;
    totalHits += 1;
    if (isCrit)
        totalCrits += 1;
    if (isPen)
        totalPens += 1;

    // log
    if (log)
        log->logHit(this, currentTime, srcSkill ? srcSkill->name : srcPassive->name, dmg, isCrit, isPen);
}

void Simulation::advanceTime(int timeIn60th)
{
    currentTime += timeIn60th;
    totalTimeAccum += timeIn60th;

    // reduce skill CDs
    for (auto& cd : skillCDs)
    {
        cd -= timeIn60th;
        if (cd < 0)
            cd = 0;
    }

    // TODO: process passives
}

void Simulation::addResource(bool currentOnly)
{
    if (currentWeapon == -1)
        return; // build on aux?

    // build primary
    if (weaponResources[currentWeapon] < 5)
        ++weaponResources[currentWeapon];

    // build secondary
    if (!currentOnly)
        if (weaponResources[1 - currentWeapon] < 5)
            ++weaponResources[1 - currentWeapon];
}
