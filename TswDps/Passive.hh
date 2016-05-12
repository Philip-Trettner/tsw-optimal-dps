#pragma once

#include "common.hh"

#include "Types.hh"
#include "Weapon.hh"
#include "Stats.hh"

struct Passive
{
    string name;
    Weapon weaponType = Weapon::None; ///< for classification
    DmgType dmgtype = DmgType::None;
    PassiveType passivetype = PassiveType::None;
    bool restrictWeapon = false; ///< if true, only weaponType weapons benefit from this passive

    Stats bonusStats;

    bool triggers = false;
};
