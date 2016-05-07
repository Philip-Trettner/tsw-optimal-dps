#pragma once

#include "common.hh"

#include "Skill.hh"
#include "Skillset.hh"
#include "Augment.hh"

class Build
{
public:
    Skillset skills;

    vector<int> rotation;

    Build();

    void loadPistolShotgunHairTrigger();

    void dump();
    
    void woodcutterDetailed();    
    float woodcutterPenChance(float penChance);
};
