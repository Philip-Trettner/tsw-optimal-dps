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
    EgonPendant,
    FuryStacks,
    FuryBuff,
    Sadism,
    Opportunism,
    SubwayTokens,
    SubwayTokensCountdown,
    Equilibrium,
    ConeyIslandBand,

    // general passives
    MinorPenetrationChance,
    MajorPenetrationChance,
    MinorCriticalChance,
    MajorCriticalChance,
    MajorHitChance,
    PenRating,
    HitRating,
    CritRating,
    CritPowerRating,

    // specific passives
    TwistTheKnife,
    Lethality,
    ElementalForceStacks,
    ElementalForceBuff,
    FatalFlourishStacks,
    FatalFlourishBuff,
    DoomStacks,
    DoomBuff,
    InvasiveMeasures,
    InvasiveMeasuresProc,
    Ferocity,

    // skill passives
    Reckless,
    AmorFati,
    FullMomentum,
    LockStockBarrel,
    LockStockBarrelGain,
    TearEmUp,
    GunFu,
    LockAndLoad,
    SteelEcho,
    Cannibalise,
    AnimaCharge,
    FireInTheHole,

    // procs
    SuddenReturn,
    OneInTheChamber,
    Thunderstruck,
    FortunateStrike,
    LiveWireStack,
    LiveWireProc,
    Gnosis,
    Tenderising,
    GrandSlam,

    // dots
    Bombardment,
    Whiteout,
    GoForTheThroat,
    EyeOfPandemonium,
    PowerLine,
    PowerLineDetonation,
    FireManifestation,
    LightningManifestation,
    Plague,
    LeftHandOfDarkness,
    Contaminate,
    ThirdDegreeStack,
    ThirdDegree,

    // weapon skills
    DoubleUp,
    Calamity,
    ElementalOverload,
    MomentumStack,
    MomentumBuff,
    BloodOffering,
    ElementalFury,

    // augments
    AugCuring,
    AugInspiring,
    AugAccelerating,
    AugQuickening,
    AugInvulnerable,
    AugSalubrious,
    AugMercurial,

    // stimulants
    StimAttackPurple,
    StimCritPurple,
    StimPenPurple,
    StimCritBlue,
    StimPenBlue,

    // kickbacks
    KickbackPenPurple,
    KickbackCritPurple,
    KickbackCritPowerPurple,
    KickbackPenBlue,
    KickbackCritBlue,
    KickbackCritPowerBlue,

    //
    );

enum class ProcOn
{
    Gain,
    SkillHit,
    Loss
};

struct Effect
{
    string name;
    EffectSlot slot = EffectSlot::Count;
    EffectSlot blockedSlot = EffectSlot::Count; // if < Count, this effect cannot trigger if that effect has Cooldown!

    Stats bonusStats;
    bool hasBonusStats;

    bool oncePerAbility = false;
    int maxStacks = 1;
    int cooldownIn60th = 60;
    int timeIn60th = 0; // 0 = no stacks, only applies once (for procs)
    bool noInitialCooldown = false;

    bool resetOnPen = false;    //< if true, resets effect on pen
    bool resetOnGlance = false; //< if true, resets effect on glance
    bool resetOnMax = false;    //< if true, resets effect on max stacks

    bool affectedByCooldownReduction = false; //< if true, this effect's CD is reduced by CD reductions

    int triggerOnMaxStacksCnt = 1; // nr of stack gained for triggerOnMaxStacks
    EffectSlot triggerOnMaxStacks
        = EffectSlot::Count; // if < Count, this effects triggers another on gaining max stacks (and looses all stack)
    EffectSlot triggerOnStackLost = EffectSlot::Count; // < if < count, triggers every time a stack is lost
    bool triggerOnStackLostOnlyLast = false;           // if true, only triggers for last hit

    DmgType dmgtype = DmgType::None; // for vulnerabilities

    float procDmgScaling = 0.f;         // if > 0, triggers a proc dmg hit everytime this effect is applied
    float procDmgFixed = 0.f;           // if > 0, triggers a fixed proc hit
    float procDmgPercentage = 0.f;      // if > 0, triggers a proc hit depending on the original hit
    bool affectedByAdditiveDmg = false; // if true, proc dmg is buffed by additive dmg (e.g. Bombardment)
    ProcOn procOn = ProcOn::Gain;       // controls when dmg procs are applied
    bool isFullHit = false;             // if true, is a full hit

    bool cannotTriggerOnPassiveFullHit = false; // if true, cannot be triggered by a passive full hit

    Stats procBonusStats;

    bool triggersGroupHealOnGain = false;  //< if true, triggers a group heal when a stack is gained (support augments)
    bool triggersPlayerHealOnLost = false; //< if true, triggers a player heal when a stack is lost (immortal spirit)

    float reducesCooldown = 0; // if > 0, reduces x % of all cooldowns

    bool affectProcs = true; // if true, bonus stats affect procs (additive dmg does not affect it anyways)

    Weapon restrictToWeapon = Weapon::None;          // if non-None, only affects the specified weapon type
    SkillType restrictToSkillType = SkillType::None; // if non-None, only affects the specified skill type

    bool affects(DmgType dmgtype, SkillType skilltype, SubType subtype, Weapon weapon) const
    {
        if (cannotTriggerOnPassiveFullHit && skilltype == SkillType::PassiveFullHit)
            return false;
        if (!affectProcs && skilltype == SkillType::Proc)
            return false;
        if (restrictToWeapon != Weapon::None && weapon != restrictToWeapon)
            return false;
        if (restrictToSkillType != SkillType::None && skilltype != restrictToSkillType)
            return false;
        // TODO: subtype and dmgtype
        return true;
    }

    bool cannotConsumeSameAbility = false;        // if true, cannot be consumed in same ability
    bool consumedAfterHit = false;                // if true, 1 stack is consumed after next hit
    bool consumedAfterAbility = false;            // if true, 1 stack is consumed after next ability
    EffectSlot gainOnConsume = EffectSlot::Count; // if non-Count, gains a stack of a given effect after consumed

    int gainResources = 0;                          // gain resources for active weapon
    bool makeConsumerFree = false;                  // if true, makes the next consumer free
    EffectSlot triggerOnMaxRes = EffectSlot::Count; // if max-res, this effect is triggered

    EffectSlot gainEffectStacks
        = EffectSlot::Count;    // if non-Count, gains stacks of a given effect when this one gains them
    int gainEffectStacksTo = 0; // if > 0, sets stack to a given value
};
