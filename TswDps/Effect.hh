#pragma once

#include <sstream>
#include "Stats.hh"
#include "common.hh"

#include "Types.hh"

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

    // skill passives
    Reckless,
    AmorFati,

    // procs
    SuddenReturn,
    OneInTheChamber,
    Thunderstruck,
    FortunateStrike,
    LiveWireStack,
    LiveWireBuff,
    Gnosis,

    // weapon skills
    DoubleUp,
    Calamity,
    ElementalOverload,

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

    DmgType dmgtype = DmgType::None; // for vulnerabilities

    float procDmgScaling = 0.f;    // if > 0, triggers a proc dmg hit everytime this effect is applied
    float procDmgPercentage = 0.f; // if > 0, triggers a proc hit depending on the original hit

    bool affectProcs = true;
};
