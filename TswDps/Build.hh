#pragma once

#include "common.hh"

#include "Skill.hh"
#include "Augment.hh"

class Build
{
public:
    vector<Skill> rotation;
    vector<Augment> augments; // corresponds to rotation

    Build();

    void loadPistolShotgunHairTrigger();

    void dump();
};
