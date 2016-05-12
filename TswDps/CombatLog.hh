#pragma once

#include "common.hh"

struct Simulation;

struct CombatLog
{
    virtual ~CombatLog();

    // is called for every active, passive, signet hit
    virtual void logHit(Simulation* sim, int timeIn60th, string const& name, float dmg, bool critical, bool penetrated)
    {
    }
    // is called for every stack gained
    virtual void logEffectStart(Simulation* sim, int timeIn60th, string const& name) {}
    // is called for every stack lost
    virtual void logEffectEnd(Simulation* sim, int timeIn60th, string const& name) {}
};

struct VerboseLog : CombatLog
{
    void logHit(Simulation* sim, int timeIn60th, string const& name, float dmg, bool critical, bool penetrated) override;
    void logEffectStart(Simulation* sim, int timeIn60th, string const& name) override;
    void logEffectEnd(Simulation* sim, int timeIn60th, string const& name) override;
};
