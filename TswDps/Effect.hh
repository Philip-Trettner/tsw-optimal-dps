#pragma once

#include <sstream>
#include "Stats.hh"
#include "common.hh"

#include "Types.hh"

#include "Weapon.hh"

#define MAKE_EFFECT_ENUM(...)                                                  \
    enum class EffectSlot                                                      \
    {                                                                          \
        __VA_ARGS__ Count                                                      \
    };                                                                         \
    inline string to_string(EffectSlot slot)                                   \
    {                                                                          \
        assert((int)slot >= 0 && slot < EffectSlot::Count && "invalid value"); \
        static std::vector<string> vals;                                       \
        if (vals.empty())                                                      \
        {                                                                      \
            string s = #__VA_ARGS__;                                           \
            std::ostringstream tmp;                                            \
            for (auto i = 0u; i < s.size(); ++i)                               \
            {                                                                  \
                if (isspace(s[i]))                                             \
                    continue;                                                  \
                else if (s[i] == ',')                                          \
                {                                                              \
                    vals.push_back(tmp.str());                                 \
                    tmp.str(string());                                         \
                }                                                              \
                else                                                           \
                    tmp << s[i];                                               \
            }                                                                  \
            vals.push_back(tmp.str());                                         \
        }                                                                      \
        return vals[(size_t)slot];                                             \
    }

MAKE_EFFECT_ENUM(
    // signets
    Laceration,
    Abuse,
    Aggression,
    MothersWrathStacks,
    MothersWrathBuff,

    // general passives
    MinorPenetrationChance,
    MajorPenetrationChance,
    MinorCriticalChance,
    MajorCriticalChance,
    CritRating,
    CritPowerRating,

    // specific passives
    TwistTheKnife,
    Lethality,
    ElementalForceStacks,
    ElementalForceBuff,
    FatalFlourishStacks,
    FatalFlourishBuff,

    // skill passives
    Reckless,
    AmorFati,
    FullMomentum,
    LockStockBarrel,
    LockStockBarrelGain,

    // procs
    SuddenReturn,
    OneInTheChamber,
    Thunderstruck,
    FortunateStrike,
    LiveWireStack,
    LiveWireProc,
    Gnosis,
	BombardmentStacks,
	Bombardment,
	Tenderising,

    // weapon skills
    DoubleUp,
    Calamity,
    ElementalOverload,
    MomentumStack,
    MomentumBuff,

    //
    );

struct Effect
{
    string name;
    EffectSlot slot = EffectSlot::Count;
    EffectSlot blockedSlot = EffectSlot::Count; // if < Count, this effect cannot trigger if that effect has Cooldown!

    Stats bonusStats;

    bool oncePerAbility = false;
    int maxStacks = 1;
    int cooldownIn60th = 60;
    int timeIn60th = 0; // 0 = no stacks, only applies once (for procs)

    bool resetOnPen = false; //< if true, resets effect on pen

    EffectSlot triggerOnMaxStacks
        = EffectSlot::Count; // if < Count, this effects triggers another on gaining max stacks (and looses all stack)
	EffectSlot triggerOnStackLost = EffectSlot::Count; // < if < count, triggers every time a stack is lost

    DmgType dmgtype = DmgType::None; // for vulnerabilities

    float procDmgScaling = 0.f;    // if > 0, triggers a proc dmg hit everytime this effect is applied
    float procDmgPercentage = 0.f; // if > 0, triggers a proc hit depending on the original hit
	bool affectedByAdditiveDmg = false; // if true, proc dmg is buffed by additive dmg (e.g. Bombardment)

    bool affectProcs = true; // if true, bonus stats affect procs (additive dmg does not affect it anyways)

    Weapon restrictToWeapon = Weapon::None;          // if non-None, only affects the specified weapon type
    SkillType restrictToSkillType = SkillType::None; // if non-None, only affects the specified skill type

    bool cannotConsumeSameAbility = false;        // if true, cannot be consumed in same ability
    bool consumedAfterHit = false;                // if true, 1 stack is consumed after next hit
    bool consumedAfterAbility = false;            // if true, 1 stack is consumed after next ability
    EffectSlot gainOnConsume = EffectSlot::Count; // if non-Count, gains a stack of a given effect after consumed

    int gainResources = 0; // gain resources for active resource

    EffectSlot gainEffectStacks
        = EffectSlot::Count;    // if non-Count, gains stacks of a given effect when this one gains them
    int gainEffectStacksTo = 0; // if > 0, sets stack to a given value
};
