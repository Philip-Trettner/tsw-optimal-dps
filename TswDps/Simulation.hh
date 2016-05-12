#pragma once

#include <random>

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
    bool resetStatsAtStart = false;      ///< if true, resets stats at the start of each "simulate"

    // gear, skills, rotation
    Skillset skills;
    SharedRotation rotation;
    Gear gear;
    Stats potionStats;
    EnemyInfo enemyInfo;

    // brief stats
    double totalDmg = 0.0f;
    int totalHits = 0;
    int totalPens = 0;
    int totalCrits = 0;
    int totalTimeAccum = 0;
    double totalDPS() const { return totalDmg / (totalTimeAccum / 60.); }
    void resetStats();

    Simulation();

    /// initializes the simulation
    /// * has to be called after changes to skills or gear
    /// * changing rotation is fine
    void init();

    /// simulate and write to log
    void simulate(int totalTimeIn60th);

    /// dumps all skill stats
    void dumpSkillStats();
    /// dumps brief report of last finished simulation
    void dumpBriefReport();

private: // run-time INIT data
    // includes total gear stats including all non-effect passives, weapons, and signets
    // includes multi-hit penalty
    Stats skillStats[SKILL_CNT];
    // weapon index per skill (-1 for aux)
    int skillWeaponIdx[SKILL_CNT];
    // penalties for multi hit and stuff
    float skillPenCritPenalty[SKILL_CNT];
    // all passives that could !trigger! for this skill
    std::vector<Passive> skillTriggers[SKILL_CNT];

private: // run-time TRANSIENT data
    // current time in 60th
    int currentTime;
    // skill cooldowns
    int skillCDs[SKILL_CNT];
    // resources for both weapons
    int weaponResources[2];
    // currently equipped weapon (0 = left, 1 = right, -1 = aux)
    int currentWeapon;
    // random
    std::default_random_engine random;

    void fullHit(Stats const& baseStats, float dmgScaling, float penCritPenalty, Skill const* srcSkill, Passive const* srcPassive);

    void advanceTime(int timeIn60th);

    void addResource(bool currentOnly); ///< to current weapon
};
