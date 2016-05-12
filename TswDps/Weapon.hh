#pragma once

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
