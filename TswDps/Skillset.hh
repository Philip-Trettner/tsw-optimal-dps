#pragma once

#include "Skill.hh"
#include "Augment.hh"
#include "Passive.hh"

struct Skillset
{
    Skill skills[8];
    Augment augments[8];
    Passive passives[8];
};
