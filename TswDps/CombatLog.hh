#pragma once

#include "common.hh"

#include "Effect.hh"
#include "Weapon.hh"

struct Simulation;
struct Stats;
struct Skill;

struct CombatLog
{
    virtual ~CombatLog();

    // is called whenever a skill is activated
    virtual void logSkill(Simulation* sim, int timeIn60th, int skillIdx) {}
    // is called for every active, passive, signet hit
    virtual void logHit(Simulation* sim,
                        int timeIn60th,
                        string const& name,
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
                        Stats const& stats,
                        float vulnMultiplier)
    {
    }
    // is called for every stack gained
    virtual void logEffectStart(Simulation* sim, int timeIn60th, EffectSlot slot) {}
    // is called for every stack lost
    virtual void logEffectEnd(Simulation* sim, int timeIn60th, EffectSlot slot) {}
    // is called whenever resources are gained or lost
    virtual void logResource(Simulation* sim, int timeIn60th, Weapon weapon, int amount) {}
};

struct VerboseLog : CombatLog
{
    bool skillsOnly = false;
    bool logEffects = true;
    bool logResources = false;

    void logSkill(Simulation* sim, int timeIn60th, int skillIdx) override;
    void logHit(Simulation* sim,
                int timeIn60th,
                string const& name,
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
                Stats const& stats,
                float vulnMultiplier) override;
    void logEffectStart(Simulation* sim, int timeIn60th, EffectSlot slot) override;
    void logEffectEnd(Simulation* sim, int timeIn60th, EffectSlot slot) override;
    void logResource(Simulation* sim, int timeIn60th, Weapon weapon, int amount) override;
};

struct StatLog : CombatLog
{
    struct DmgStat
    {
        double totalDmg = 0.0;
        int hits = 0;
        int pens = 0;
        int crits = 0;
        int glances = 0;
        int blocks = 0;
        int evades = 0;
    };

    std::map<std::string, DmgStat> dmgStats;
    std::map<DmgType, double> dmgOfType;
    std::map<SkillType, double> dmgOfSkill;
    std::map<SubType, double> dmgOfSub;
    std::map<Weapon, double> dmgOfWeapon;

    void logHit(Simulation* sim,
                int timeIn60th,
                string const& name,
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
                Stats const& stats,
                float vulnMultiplier) override
    {
        auto& s = dmgStats[name];
        s.totalDmg += dmg;
        s.hits += 1;
        if (critical)
            s.crits += 1;
        if (penetrated)
            s.pens += 1;
        if (glanced)
            s.glances += 1;
        if (blocked)
            s.blocks += 1;
        if (evaded)
            s.evades += 1;

        dmgOfType[dmgType] += dmg;
        dmgOfSkill[skillType] += dmg;
        dmgOfSub[subType] += dmg;
        dmgOfWeapon[weaponType] += dmg;
    }

    void dump(Simulation* sim);

    DmgStat operator[](Skill const& s) const;
    DmgStat operator[](EffectSlot s) const;
};

struct AggregateLog : CombatLog
{
    std::vector<CombatLog*> logs;

    void logSkill(Simulation* sim, int timeIn60th, int skillIdx) override
    {
        for (auto log : logs)
            log->logSkill(sim, timeIn60th, skillIdx);
    }
    void logHit(Simulation* sim,
                int timeIn60th,
                string const& name,
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
                Stats const& stats,
                float vulnMultiplier) override
    {
        for (auto log : logs)
            log->logHit(sim, timeIn60th, name, dmg, critical, penetrated, glanced, blocked, evaded, dmgType, skillType,
                        subType, weaponType, stats, vulnMultiplier);
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
    void logResource(Simulation* sim, int timeIn60th, Weapon weapon, int amount) override
    {
        for (auto log : logs)
            log->logResource(sim, timeIn60th, weapon, amount);
    }
};
