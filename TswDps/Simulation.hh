#pragma once

#include "common.hh"
#include "Skillset.hh"
#include "Stats.hh"
#include "Rotation.hh"
#include "Gear.hh"
#include "EnemyInfo.hh"

struct CombatLog;

struct Simulation
{
    // common setup
    CombatLog* log = nullptr;
    bool warnOnWait = true;       ///< warns if sim waits for a CD
    bool lowVarianceMode = false; ///< if true, tries to reduce variance per sim

    // gear, skills, rotation
    Skillset skills;
    SharedRotation rotation;
    Gear gear;
    Stats potionStats;
    EnemyInfo enemyInfo;

    Simulation();

    /// initializes the simulation
    /// * has to be called after changes to skills or gear
    /// * changing rotation is fine
    void init();

    /// simulate and write to log
    void simulate(int totalTimeIn60th);

    /// dumps all skill stats
    void dumpSkillStats();

private: // run-time data
    // includes total gear stats including all non-effect passives, weapons, and signets
    // includes multi-hit penalty
    Stats skillStats[SKILL_CNT];
    // all passives that could !trigger! for this skill
    std::vector<Passive> skillTriggers[SKILL_CNT];
    // current time in 60th
    int currentTime;
    // skill cooldowns
    int skillCDs[SKILL_CNT];

    void advanceTime(int timeIn60th);
};
