#pragma once

#include "Stats.hh"

struct EnemyInfo
{
    Stats stats;

    float penPower = .49f + .24f;        // normal boss + breaching
    float baseVulnerability = .02f * 10; // expose
};
