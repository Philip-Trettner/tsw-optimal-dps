#include "Simulation.hh"

#include <iostream>

#include "CombatLog.hh"

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
            s.weaponPower = 457;
        else if (skill.weapon == gear.leftWeapon || skill.weapon == gear.rightWeapon)
        {
            auto const& wp = skill.weapon == gear.leftWeapon ? gear.pieces[Gear::WeaponLeft] : gear.pieces[Gear::WeaponRight];
            s = s + wp.stats;
            passives.push_back(wp.signet.effect);
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

        // extract trigger passives
        for (auto const& passive : passives)
            if (passive.triggers)
                triggers.push_back(passive);

        skillStats[i] = s;
        skillTriggers[i] = triggers;
    }
}

void Simulation::simulate(int totalTimeIn60th)
{
    assert(rotation && "no rotation set");

    // reset sim
    rotation->reset();
    currentTime = 0;
    for (auto i = 0; i < SKILL_CNT; ++i)
        skillCDs[i] = 0;

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

        auto stats = skillStats[idx]; // copy
        // TODO: add currently running effects
        stats.update(enemyInfo);

        auto remainingTime = skill.timeIn60th;

        // casttime
        advanceTime(skill.casttimeIn60th);
        remainingTime -= skill.casttimeIn60th;

        // actually do skill
        for (auto hitIdx = 0; hitIdx < skill.hits; ++hitIdx)
        {
            // if channeling, advance time first
            if (skill.channeling)
            {
                advanceTime(skill.timeIn60th / skill.hits);
                remainingTime -= skill.timeIn60th / skill.hits;
            }

            // TODO: do a hit
            if (log)
                log->logHit(this, currentTime, skill.name, skill.dmgScaling * stats.finalCombatPower, false, false);
        }

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

void Simulation::advanceTime(int timeIn60th)
{
    currentTime += timeIn60th;

    // reduce skill CDs
    for (auto& cd : skillCDs)
    {
        cd -= timeIn60th;
        if (cd < 0)
            cd = 0;
    }

    // TODO: process passives
}
