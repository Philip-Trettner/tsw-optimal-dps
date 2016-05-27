#pragma once

#include "common.hh"

enum class Weapon
{
    None,

    Blade,
    Fist,
    Hammer,

    Blood,
    Chaos,
    Elemental,

    Rifle,
    Pistol,
    Shotgun,

    Aux,

    Count
};

inline bool startsWithResources(Weapon w)
{
    switch (w)
    {
    case Weapon::Blade:
    case Weapon::Fist:
    case Weapon::Hammer:
        return true;
    default:
        return false;
    }
}

inline string to_string(Weapon w)
{
    switch (w)
    {
    case Weapon::Blade:
        return "Blade";
    case Weapon::Hammer:
        return "Hammer";
    case Weapon::Fist:
        return "Fist";

    case Weapon::Chaos:
        return "Chaos";
    case Weapon::Blood:
        return "Blood";
    case Weapon::Elemental:
        return "Elemental";

    case Weapon::Rifle:
        return "Rifle";
    case Weapon::Pistol:
        return "Pistol";
    case Weapon::Shotgun:
        return "Shotgun";

    case Weapon::Aux:
        return "Aux";

    default:
        return "INVALID";
    }
}
