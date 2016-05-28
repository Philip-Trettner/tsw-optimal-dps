#pragma once

#include "common.hh"

#include "Effect.hh"
#include "Stats.hh"
#include "Types.hh"
#include "Weapon.hh"

enum class Trigger
{
    None,
    Hit,
    Crit,
    Pen,
    CritPen,
    FinishActivation,
	StartActivation
};

struct Passive
{
    string name;
    Weapon weaponType = Weapon::None; ///< for classification
    DmgType dmgtype = DmgType::None;
    PassiveType passivetype = PassiveType::None;
    bool restrictWeapon = false;              ///< if true, only weaponType weapons benefit from this passive
	SkillType restrictType = SkillType::None; ///< if non-none, restricts this passive to a given skill type
	SubType restrictSubType = SubType::None; ///< if non-none, restricts this passive to a given skill type

    Stats bonusStats;

    Trigger trigger = Trigger::None;
    float triggerChance = 1.f;
    EffectSlot effect = EffectSlot::Count; ///< trigger effect
	int effectStacks = 1;

    EffectSlot abilityBlockedEffect
        = EffectSlot::Count; ///< cannot trigger if it's the same ability as the blocked effect

    bool skillPassive = false; ///< if true, this is a builtin passive of a skill (Automatically set)

    bool affectsProcs() const
    {
        if (restrictType != SkillType::None)
            return false;
        if (restrictWeapon)
            return false;
        if (skillPassive)
            return false;
        return true;
    }
};
