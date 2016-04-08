#pragma once

#include "common.hh"

#include "Weapon.hh"

enum class Subtype {
    None,
    Burst,
    Strike,
    Focus,
    Frenzy
};

struct Skill
{
    string name;
    Weapon weapon;
    float time;
    int hits;
    float baseDmg;
    Subtype subtype;
};
