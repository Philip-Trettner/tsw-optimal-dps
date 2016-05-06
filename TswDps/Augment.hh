#pragma once

#include "common.hh"

/// def ctor is zero augment
struct Augment
{
    string name;
    float addPenChance = 0.f;  // .05 = 5%
    float addCritChance = 0.f; // .05 = 5%
    float addCritDamage = 0.f; // .15 = 15%
    float addDamage = 0.f;     // .05 = 5%

    int addAttackRating = 0;
    int addHealRating = 0;
    int addHP = 0;

    Augment() = default;
    Augment(string const& n) { name = n; }
};
