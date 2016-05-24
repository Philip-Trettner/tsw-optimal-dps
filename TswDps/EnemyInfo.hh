#pragma once

#include "Stats.hh"

struct EnemyInfo
{
    Stats stats;

    float penPower = .49f + .24f;        // normal boss + breaching
    float baseVulnerability = .02f * 10; // expose

    bool allVulnerabilities = false; // if true, assumes total vuln all the time
};
