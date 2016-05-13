#pragma once

#include "common.hh"

#include "Effect.hh"

struct Simulation;
struct Stats;

struct CombatLog
{
    virtual ~CombatLog();

    // is called for every active, passive, signet hit
    virtual void logHit(Simulation* sim, int timeIn60th, string const& name, float dmg, bool critical, bool penetrated, Stats const& stats)
    {
    }
    // is called for every stack gained
    virtual void logEffectStart(Simulation* sim, int timeIn60th, EffectSlot slot) {}
    // is called for every stack lost
    virtual void logEffectEnd(Simulation* sim, int timeIn60th, EffectSlot slot) {}
};

struct VerboseLog : CombatLog
{
    void logHit(Simulation* sim, int timeIn60th, string const& name, float dmg, bool critical, bool penetrated, Stats const& stats) override;
    void logEffectStart(Simulation* sim, int timeIn60th, EffectSlot slot) override;
    void logEffectEnd(Simulation* sim, int timeIn60th, EffectSlot slot) override;
};
