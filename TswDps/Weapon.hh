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

inline Weapon parseWeapon(string const& s)
{
    if (s == "Blade")
        return Weapon::Blade;
    else if (s == "Fist")
        return Weapon::Fist;
    else if (s == "Hammer")
        return Weapon::Hammer;

    else if (s == "Chaos")
        return Weapon::Chaos;
    else if (s == "Blood")
        return Weapon::Blood;
    else if (s == "Elemental")
        return Weapon::Elemental;

    else if (s == "Rifle")
        return Weapon::Rifle;
    else if (s == "Pistol")
        return Weapon::Pistol;
    else if (s == "Shotgun")
        return Weapon::Shotgun;

    else
        return Weapon::None;
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
