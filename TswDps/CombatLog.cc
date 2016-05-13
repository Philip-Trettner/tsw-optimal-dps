#include "CombatLog.hh"

#include <iostream>

CombatLog::~CombatLog()
{
}

void VerboseLog::logHit(Simulation *sim, int timeIn60th, const string &name, float dmg, bool critical, bool penetrated, const Stats &stats)
{
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
    auto frac = (timeIn60th % 60) * 100 / 60;
    std::cout << "[" << timeIn60th / 60 << ":" << (frac < 10 ? "0" : "") << (timeIn60th % 60) * 100 / 60 << "] You gain '";
    std::cout << to_string(slot) << "'." << std::endl;
}

void VerboseLog::logEffectEnd(Simulation *sim, int timeIn60th, EffectSlot slot)
{
    auto frac = (timeIn60th % 60) * 100 / 60;
    std::cout << "[" << timeIn60th / 60 << ":" << (frac < 10 ? "0" : "") << (timeIn60th % 60) * 100 / 60 << "] You lose '";
    std::cout << to_string(slot) << "'." << std::endl;
}
