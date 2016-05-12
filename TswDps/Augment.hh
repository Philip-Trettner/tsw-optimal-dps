#pragma once

#include "common.hh"
#include "Stats.hh"

/// def ctor is zero augment
struct Augment
{
    string name;
    Stats bonusStats;

    // TODO: effects?

    bool affectEverything = false;

    Augment() = default;
    Augment(string const& n) { name = n; }
};
