#pragma once

#include "common.hh"
#include "Stats.hh"

enum class AugmentSlot
{
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

    // TODO: effects?

    bool affectEverything = false;

    Augment() = default;
    Augment(string const& n) { name = n; }
};
