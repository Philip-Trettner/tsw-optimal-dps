#pragma once

#include "common.hh"

#include "Effect.hh"

struct Simulation;
struct Stats;

struct CombatLog
{
    bool skillsOnly = false;

    virtual ~CombatLog();

    // is called whenever a skill is activated
    virtual void logSkill(Simulation* sim, int timeIn60th, int skillIdx) {}
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
    void logSkill(Simulation* sim, int timeIn60th, int skillIdx) override;
    void logHit(Simulation* sim, int timeIn60th, string const& name, float dmg, bool critical, bool penetrated, Stats const& stats) override;
    void logEffectStart(Simulation* sim, int timeIn60th, EffectSlot slot) override;
    void logEffectEnd(Simulation* sim, int timeIn60th, EffectSlot slot) override;
};

struct StatLog : CombatLog
{
    struct DmgStat
    {
        double totalDmg = 0.0;
        int hits = 0;
        int pens = 0;
        int crits = 0;
    };

    std::map<std::string, DmgStat> dmgs;

    virtual void logHit(Simulation* sim, int timeIn60th, string const& name, float dmg, bool critical, bool penetrated, Stats const& stats)
    {
        auto& s = dmgs[name];
        s.totalDmg += dmg;
        s.hits += 1;
        if (critical)
            s.crits += 1;
        if (penetrated)
            s.pens += 1;
    }

    void dump(Simulation* sim);
};

struct AggregateLog : CombatLog
{
    std::vector<CombatLog*> logs;

    void logSkill(Simulation* sim, int timeIn60th, int skillIdx) override
    {
        for (auto log : logs)
            log->logSkill(sim, timeIn60th, skillIdx);
    }
    void logHit(Simulation* sim, int timeIn60th, string const& name, float dmg, bool critical, bool penetrated, Stats const& stats) override
    {
        for (auto log : logs)
            log->logHit(sim, timeIn60th, name, dmg, critical, penetrated, stats);
    }
    void logEffectStart(Simulation* sim, int timeIn60th, EffectSlot slot) override
    {
        for (auto log : logs)
            log->logEffectStart(sim, timeIn60th, slot);
    }
    void logEffectEnd(Simulation* sim, int timeIn60th, EffectSlot slot) override
    {
        for (auto log : logs)
            log->logEffectEnd(sim, timeIn60th, slot);
    }
};
