#pragma once

#include "common.hh"

#include "Weapon.hh"

enum class SubType {
    None,
    Burst,
    Strike,
    Focus,
    Frenzy
};

enum class DmgType {
    Melee,
    Magic,
    Ranged
};

struct Skill
{
    string name;
    Weapon weapon;
    float time;
    int hits;
    float baseDmg;
    SubType subtype;
    DmgType dmgtype;
};
