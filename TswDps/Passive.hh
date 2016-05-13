#pragma once

#include "common.hh"

#include "Types.hh"
#include "Weapon.hh"
#include "Stats.hh"
#include "Effect.hh"

enum class Trigger
{
    None,
    Hit,
    Crit,
    Pen,
    CritPen
};

struct Passive
{
    string name;
    Weapon weaponType = Weapon::None; ///< for classification
    DmgType dmgtype = DmgType::None;
    PassiveType passivetype = PassiveType::None;
    bool restrictWeapon = false; ///< if true, only weaponType weapons benefit from this passive
    SkillType restrictType = SkillType::None; ///< if non-none, restricts this passive to a given skill type

    Stats bonusStats;

    Trigger trigger = Trigger::None;
    float triggerChance = 1.f;
    Effect effect; ///< trigger effect
};
