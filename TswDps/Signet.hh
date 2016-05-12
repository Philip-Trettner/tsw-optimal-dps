#pragma once

#include "common.hh"
#include "Types.hh"
#include "Passive.hh"

enum class SignetSlot
{
    None,
    Head,
    Major,
    Minor,
    HeadWeapon
};

struct Signet
{
    SignetSlot slot = SignetSlot::None;
    Passive effect;

    string const& name() const { return effect.name; }
};
