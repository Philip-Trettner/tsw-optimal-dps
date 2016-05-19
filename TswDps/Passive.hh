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
    FinishActivation
};

struct Passive
{
    string name;
    Weapon weaponType = Weapon::None; ///< for classification
    DmgType dmgtype = DmgType::None;
    PassiveType passivetype = PassiveType::None;
    bool restrictWeapon = false;              ///< if true, only weaponType weapons benefit from this passive
    SkillType restrictType = SkillType::None; ///< if non-none, restricts this passive to a given skill type

    Stats bonusStats;

    Trigger trigger = Trigger::None;
    float triggerChance = 1.f;
    EffectSlot effect; ///< trigger effect

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
