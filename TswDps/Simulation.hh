#pragma once

#include "Skillset.hh"
#include "Stats.hh"
#include "Rotation.hh"

struct Simulation
{
    Skillset skills;
    SharedRotation rotation;
    Stats baseStats;

    Stats weaponStats[(int)Weapon::Count];

    Simulation();
};
