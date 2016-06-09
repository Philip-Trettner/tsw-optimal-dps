#include "CombatLog.hh"

#include "Simulation.hh"

#include <algorithm>

#include <iostream>
#include <iomanip>

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
        std::cout.width(4);
        std::cout << std::right << std::fixed << std::setprecision(1) << int(s.totalDmg * 1000 / totalDmg) / 10.;
        std::cout << "% '" << kvp.first << "' (" << s.hits << " hits, " << int(s.crits * 1000. / s.hits) / 10.
                  << "% crits, " << int(s.pens * 1000. / s.hits) / 10. << "% pens)" << std::endl;
    }
}

StatLog::DmgStat StatLog::operator[](const Skill &s) const
{
    if (dmgStats.count(s.name))
        return dmgStats.at(s.name);
    return {};
}

StatLog::DmgStat StatLog::operator[](EffectSlot s) const
{
    auto n = to_string(s);
    if (dmgStats.count(n))
        return dmgStats.at(n);
    return {};
}
