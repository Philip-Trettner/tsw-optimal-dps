#include "CombatLog.hh"

#include <iostream>

CombatLog::~CombatLog()
{
}

void VerboseLog::logHit(Simulation *sim, int timeIn60th, const string &name, float dmg, bool critical, bool penetrated)
{
    auto frac = (timeIn60th % 60) * 100 / 60;
    std::cout << "[" << timeIn60th / 60 << ":" << (frac < 10 ? "0" : "") << (timeIn60th % 60) * 100 / 60 << "] '";
    std::cout << name << "' hit for " << dmg << " (";
    if (critical && penetrated)
        std::cout << "crit-pen";
    else if (critical)
        std::cout << "critical";
    else if (penetrated)
        std::cout << "penetrated";
    else
        std::cout << "normal";
    std::cout << ")." << std::endl;
}

void VerboseLog::logEffectStart(Simulation *sim, int timeIn60th, const string &name)
{
    auto frac = (timeIn60th % 60) * 100 / 60;
    std::cout << "[" << timeIn60th / 60 << ":" << (frac < 10 ? "0" : "") << (timeIn60th % 60) * 100 / 60 << "] ";
    std::cout << name << " starts.";
}

void VerboseLog::logEffectEnd(Simulation *sim, int timeIn60th, const string &name)
{
    auto frac = (timeIn60th % 60) * 100 / 60;
    std::cout << "[" << timeIn60th / 60 << ":" << (frac < 10 ? "0" : "") << (timeIn60th % 60) * 100 / 60 << "] ";
    std::cout << name << " end.";
}
