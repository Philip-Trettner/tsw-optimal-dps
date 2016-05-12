#pragma once

#include "common.hh"
#include "Skill.hh"
#include "Augment.hh"
#include "Passive.hh"

struct Skillset
{
    Skill skills[SKILL_CNT];
    Augment augments[SKILL_CNT];
    Passive passives[SKILL_CNT];
};
