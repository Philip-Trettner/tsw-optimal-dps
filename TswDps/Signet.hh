#pragma once

#include "common.hh"
#include "Types.hh"
#include "Passive.hh"

enum class SignetSlot
{
    Head,
    Major,
    Minor,
    HeadWeapon
};

struct Signet
{
    SignetSlot slot;
    Passive effect;

    string const& name() const { return effect.name; }
};
