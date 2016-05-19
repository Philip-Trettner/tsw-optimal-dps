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

void VerboseLog::logHit(Simulation *sim, int timeIn60th, const string &name, float dmg, bool critical, bool penetrated, const Stats &stats)
{
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
    std::cout << ", dmg x" << stats.finalDmgMultiplier;
    std::cout << ")." << std::endl;
}

void VerboseLog::logEffectStart(Simulation *sim, int timeIn60th, EffectSlot slot)
{
    return;
    auto frac = (timeIn60th % 60) * 100 / 60;
    std::cout << "[" << timeIn60th / 60 << ":" << (frac < 10 ? "0" : "") << (timeIn60th % 60) * 100 / 60
              << "] You gain '";
    std::cout << to_string(slot) << "'." << std::endl;
}

void VerboseLog::logEffectEnd(Simulation *sim, int timeIn60th, EffectSlot slot)
{
    return;
    auto frac = (timeIn60th % 60) * 100 / 60;
    std::cout << "[" << timeIn60th / 60 << ":" << (frac < 10 ? "0" : "") << (timeIn60th % 60) * 100 / 60
              << "] You lose '";
    std::cout << to_string(slot) << "'." << std::endl;
}

void StatLog::dump(Simulation *sim)
{
    std::vector<std::pair<string, DmgStat>> stats;
    for (auto const &kvp : dmgs)
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
