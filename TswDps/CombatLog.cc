#include "CombatLog.hh"

#include "Simulation.hh"

#include <algorithm>

#include <iomanip>
#include <iostream>

CombatLog::~CombatLog()
{
}

void VerboseLog::logSkill(Simulation *sim, int timeIn60th, int skillIdx)
{
    auto frac = (timeIn60th % 60) * 100 / 60;
    std::cout << "[" << timeIn60th / 60 << ":" << (frac < 10 ? "0" : "") << (timeIn60th % 60) * 100 / 60
              << "] You activate '";
    std::cout << sim->skills.skills[skillIdx].name << "'." << std::endl;
}

void VerboseLog::logHit(Simulation *sim,
                        int timeIn60th,
                        const string &name,
                        float dmg,
                        bool critical,
                        bool penetrated,
                        bool glanced,
                        bool blocked,
                        bool evaded,
                        DmgType dmgType,
                        SkillType skillType,
                        SubType subType,
                        Weapon weaponType,
                        const Stats &stats,
                        float vulnMultiplier)
{
    if (skillsOnly)
        return;

    auto frac = (timeIn60th % 60) * 100 / 60;
    std::cout << "[" << timeIn60th / 60 << ":" << (frac < 10 ? "0" : "") << (timeIn60th % 60) * 100 / 60 << "] '";
    std::cout << name << "' hit for " << dmg << " (";
    if (critical && penetrated)
        std::cout << "CRIT-PEN, crit +" << stats.finalCritPower * 100 << "%";
    else if (critical)
        std::cout << "critical, crit +" << stats.finalCritPower * 100 << "%";
    else if (penetrated)
        std::cout << "penetrated";
    else
        std::cout << "normal";
    if (glanced)
        std::cout << ", glanced";
    if (blocked)
        std::cout << ", blocked";
    if (evaded)
        std::cout << ", evaded";
    std::cout << ", dmg x" << stats.finalDmgMultiplier;
    std::cout << ", vuln x" << vulnMultiplier;
    std::cout << ")." << std::endl;
}

void VerboseLog::logEffectStart(Simulation *sim, int timeIn60th, EffectSlot slot)
{
    if (skillsOnly)
        return;
    if (!logEffects)
        return;

    auto frac = (timeIn60th % 60) * 100 / 60;
    std::cout << "[" << timeIn60th / 60 << ":" << (frac < 10 ? "0" : "") << (timeIn60th % 60) * 100 / 60
              << "] You gain '";
    std::cout << to_string(slot) << "'." << std::endl;
}

void VerboseLog::logEffectEnd(Simulation *sim, int timeIn60th, EffectSlot slot)
{
    if (skillsOnly)
        return;
    if (!logEffects)
        return;

    auto frac = (timeIn60th % 60) * 100 / 60;
    std::cout << "[" << timeIn60th / 60 << ":" << (frac < 10 ? "0" : "") << (timeIn60th % 60) * 100 / 60
              << "] You lose '";
    std::cout << to_string(slot) << "'." << std::endl;
}

void VerboseLog::logResource(Simulation *sim, int timeIn60th, Weapon weapon, int amount)
{
    if (skillsOnly)
        return;
    if (!logResources)
        return;

    auto frac = (timeIn60th % 60) * 100 / 60;
    std::cout << "[" << timeIn60th / 60 << ":" << (frac < 10 ? "0" : "") << (timeIn60th % 60) * 100 / 60
              << "] You gain ";
    std::cout << amount << " resource(s) for " << to_string(weapon);

    std::cout << "." << std::endl;
}

void VerboseLog::logHeal(Simulation *sim, int timeIn60th, EffectSlot slot, bool criticalSelf, bool criticalAny, int affected)
{
    if (skillsOnly)
        return;
    if (!logEffects)
        return;

    auto frac = (timeIn60th % 60) * 100 / 60;
    std::cout << "[" << timeIn60th / 60 << ":" << (frac < 10 ? "0" : "") << (timeIn60th % 60) * 100 / 60 << "] '";
    std::cout << to_string(slot) << "' heals " << affected << " member (self: ";
    std::cout << (criticalSelf ? "critical" : "normal") << ", any: ";
    std::cout << (criticalAny ? "critical" : "normal");
    std::cout << ")." << std::endl;
}

void StatLog::dump(Simulation *sim)
{
    std::vector<std::pair<string, DmgStat>> stats;
    for (auto const &kvp : dmgStats)
        stats.push_back(kvp);
    sort(begin(stats), end(stats), [](std::pair<string, DmgStat> const &s1, std::pair<string, DmgStat> const &s2) -> bool
         {
             return s1.second.totalDmg > s2.second.totalDmg;
         });

    auto totalDmg = sim->totalDmg;
    std::cout << "DPS: " << sim->totalDPS() << std::endl;
    for (auto const &kvp : stats)
    {
        auto const &s = kvp.second;
        std::cout << "  - ";
        std::cout.width(5);
        std::cout << std::right << std::fixed << std::setprecision(2) << s.totalDmg * 100. / totalDmg;
        std::cout << "% '" << kvp.first << "' (" << s.hits << " hits, " << int(s.crits * 1000. / s.hits) / 10.
                  << "% crits, " << int(s.pens * 1000. / s.hits) / 10. << "% pens)" << std::endl;
    }

    std::cout << std::endl;
    std::cout << "By Type:" << std::endl;
    std::vector<std::pair<double, std::string>> typeDmg = {
        {-dmgOfType[DmgType::Melee], "Melee"},                                   //
        {-dmgOfType[DmgType::Magic], "Magic"},                                   //
        {-dmgOfType[DmgType::Ranged], "Ranged"},                                 //
        {-dmgOfSkill[SkillType::Builder], "Builder"},                            //
        {-dmgOfSkill[SkillType::Consumer], "Consumer"},                          //
        {-dmgOfSub[SubType::Burst], "Burst"},                                    //
        {-dmgOfSub[SubType::Focus], "Focus"},                                    //
        {-dmgOfSub[SubType::Strike], "Strike"},                                  //
        {-dmgOfWeapon[sim->gear.leftWeapon], to_string(sim->gear.leftWeapon)},   //
        {-dmgOfWeapon[sim->gear.rightWeapon], to_string(sim->gear.rightWeapon)}, //
    };
    sort(begin(typeDmg), end(typeDmg));
    for (auto const &kvp : typeDmg)
    {
        if (kvp.first == 0)
            continue;

        std::cout << "  - ";
        std::cout.width(5);
        std::cout << std::right << std::fixed << std::setprecision(2) << -kvp.first * 100. / totalDmg;
        std::cout << "% " << kvp.second << std::endl;
    }
}

jsonxx::Object StatLog::dmgBreakdown()
{
    jsonxx::Object o;

    auto totalDmg = sim->totalDmg;

    // by source
    {
        jsonxx::Object d;

        for (auto const &kvp : dmgStats)
        {
            auto const &s = kvp.second;

            jsonxx::Object v;
            v << "Hits" << s.hits / (double)sim->totalHits;
            v << "Pens" << s.pens / (double)s.hits;
            v << "Crits" << s.crits / (double)s.hits;
            v << "Glances" << s.glances / (double)s.hits;
            v << "Blocks" << s.blocks / (double)s.hits;
            v << "Evades" << s.evades / (double)s.hits;
            v << "Dmg" << s.totalDmg / totalDmg;

            d << kvp.first << v;
        }

        o << "By Source" << d;
    }

    // by type
    {
        jsonxx::Object d;

        d << "Melee" << dmgOfType[DmgType::Melee] / totalDmg;
        d << "Magic" << dmgOfType[DmgType::Magic] / totalDmg;
        d << "Ranged" << dmgOfType[DmgType::Ranged] / totalDmg;
        d << "Builder" << dmgOfSkill[SkillType::Builder] / totalDmg;
        d << "Consumer" << dmgOfSkill[SkillType::Consumer] / totalDmg;
        d << "Elite" << dmgOfSkill[SkillType::Elite] / totalDmg;
        d << "Burst" << dmgOfSub[SubType::Burst] / totalDmg;
        d << "Focus" << dmgOfSub[SubType::Focus] / totalDmg;
        d << "Strike" << dmgOfSub[SubType::Strike] / totalDmg;
        d << to_string(sim->gear.leftWeapon) << dmgOfWeapon[sim->gear.leftWeapon] / totalDmg;
        d << to_string(sim->gear.rightWeapon) << dmgOfWeapon[sim->gear.rightWeapon] / totalDmg;

        o << "By Type" << d;
    }

    return o;
}

StatLog::DmgStat StatLog::operator[](const Skill &s) const
{
    if (dmgStats.count(s.name))
        return dmgStats.at(s.name);
    return {};
}

StatLog::DmgStat StatLog::operator[](EffectSlot s) const
{
    assert(sim && "no sim attached");
    auto n = sim->effectName(s);
    if (dmgStats.count(n))
        return dmgStats.at(n);
    return {};
}
