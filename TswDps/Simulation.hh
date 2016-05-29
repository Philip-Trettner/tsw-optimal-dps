#pragma once

#include <random>

#include "common.hh"
#include "Skillset.hh"
#include "Stats.hh"
#include "Rotation.hh"
#include "Gear.hh"
#include "EnemyInfo.hh"
#include "Effect.hh"
#include "Build.hh"

struct CombatLog;

struct Simulation
{
    // common setup
    CombatLog* log = nullptr;
    bool warnOnWait = true;          ///< warns if sim waits for a CD
    bool lowVarianceMode = false;    ///< if true, tries to reduce variance per sim
    bool resetStatsAtStart = false;  ///< if true, resets stats at the start of each "simulate"
	bool stochasticLowHealth = false; ///< if true, treats bosses as stochastically low on health (below 35%), otherwise no low health

    // gear, skills, rotation
    Skillset skills;
    SharedRotation rotation;
    Gear gear;
    Stats potionStats;
    EnemyInfo enemyInfo;
    int buffAt = 5;                  ///< initial buff timing
    int dabsCDIn60th = 60 * 60;      ///< DABS are fired every x seconds
    int dabsVarianceIn60th = 5 * 60; ///< DABS may be delayed up to y seconds

    // brief stats
    double totalDmg = 0.0f;
    int totalHits = 0;
    int totalPens = 0;
    int totalCrits = 0;
    int totalTimeAccum = 0;
    double totalDPS() const { return totalDmg / (totalTimeAccum / 60.); }
    void resetStats();

    Simulation();

    /// Loads a given build
    void loadBuild(Build const& b);

    /// Returns the currently set build
    Build build() const;

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

    /// Analyzes the total dmg increase of each passive
    void analyzePassiveContribution(int maxTime = 100 * 1000 * 60);

    /// next dabs in 60th
    int nextDABS() const { return dabsTime; }
    /// checks if a given effect is active
    bool isEffectActive(EffectSlot slot) const { return effectStacks[(int)slot] > 0; }
    /// returns true if skill is off CD
    bool isSkillReady(int idx) const { return skillCDs[idx] == 0; }
    /// returns the amount of resources for a given weapon
    int resourcesFor(Weapon w) const
    {
        return w == gear.leftWeapon ? weaponResources[0] : w == gear.rightWeapon ? weaponResources[1] : -1;
    }

private: // run-time INIT data
    // includes total gear stats including all non-effect passives, weapons, and signets
    // does NOT include multi-hit penalty
    Stats skillStats[SKILL_CNT];
    Stats procStats[SKILL_CNT]; // basically proc base stats depending on weapon, does not include multi-hit penalties
                                // or non-proc affecting passives
    // weapon index per skill (-1 for aux)
    int skillWeaponIdx[SKILL_CNT];
    // penalties for multi hit and stuff
    float skillPenCritPenalty[SKILL_CNT];
    // all passives that could !trigger! for this skill
	std::vector<Passive> skillTriggers[SKILL_CNT];
	// all passives that could !trigger! for this skill when triggered by a proc
	std::vector<Passive> procTriggers[SKILL_CNT];

	// AD 
	Effect animaDeviationEffect;
	float animaDeviationScaling = -1;

private: // run-time TRANSIENT data
    // unique hit id for FULL hits only
    int currHitID;
    // unique id for each skill used
    int currSkillID;
    // current time in 60th
    int currentTime;
    // buff time
    int dabsTime;
    // skill cooldowns
    int skillCDs[SKILL_CNT];
    // effect time, CD, stacks
    int effectTime[(int)EffectSlot::Count];
    int effectCD[(int)EffectSlot::Count];
    int effectStacks[(int)EffectSlot::Count];
    int effectHitID[(int)EffectSlot::Count]; // id of hit that applied this effect
    int effectSkillID[(int)EffectSlot::Count]; // id of skill that applied this effect
    Effect effects[(int)EffectSlot::Count];
    bool effectsRegistered = false;
    // resources for both weapons
    int weaponResources[2];
    // currently equipped weapon (0 = left, 1 = right, -1 = aux)
    int currentWeapon;
    // currently used skill
    int currentSkill;
    // vulnerabilities
    int vulnTime[(int)DmgType::Count];
    float vulnDmg[(int)DmgType::Count];
    // random
    std::default_random_engine random;

    void fullHit(Stats const& baseStats,
                 Stats const& procStat,
                 float dmgScaling,
                 float penCritPenalty,
                 bool startOfAbility,
                 bool endOfAbility,
                 Skill const* srcSkill,
                 Effect const* srcEffect);
    void rawHit(Stats const& actualStats, float dmgScaling, float penCritPenalty, DmgType dmgType, bool* isCrit, bool* isPen, Skill const* srcSkill, Effect const* srcEffect);
    void procEffect(Stats const& procStats, Passive const& passive, float originalHitScaling);
	void procEffect(Stats const& procStats, EffectSlot effectSlot, float originalHitScaling);
	void procEffectDmg(Stats const& procStats, Effect const& effect, float originalHitScaling);

    void advanceTime(int timeIn60th);

    void addResource(bool currentOnly); ///< to current weapon

    void applyEffects(Stats& stats, DmgType dmgtype, SkillType skilltype, SubType subtype, Weapon weapon);

    /// resets a complete effect
    void resetEffect(EffectSlot slot);

    // has to be done for non-direct effects like the EF buff
    void registerEffect(Effect const& e);
    void registerEffects();
};
