#pragma once

#include "common.hh"

#include "Types.hh"
#include "Weapon.hh"
#include "Stats.hh"

struct Passive
{
    string name;
    Weapon weapon;
    DmgType dmgtype;
    PassiveType passivetype;

    Stats bonusStats;
};
