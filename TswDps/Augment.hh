#pragma once

#include "common.hh"
#include "Effect.hh"
#include "Passive.hh"

enum class AugmentSlot
{
    Invalid,

    Damage,
    Heal,
    Support,
    Survival
};

/// def ctor is zero augment
struct Augment
{
    string name;
    Stats bonusStats;

    AugmentSlot slot = AugmentSlot::Damage;

    EffectSlot effect = EffectSlot::Count;

    bool applyBeforeCD = false; // if true, applies before CD, TODO: CHECK IF THIS IS ACTUALLY TRUE FOR CASTTIME ABILITIES

    bool affectEverything = false;

    Augment() = default;
    Augment(string const& n) { name = n; }
};
