#pragma once

#include "Stats.hh"

struct EnemyInfo
{
    Stats stats;

    float penPower = .49f + .24f;        // normal boss + breaching
    float baseVulnerability = .02f * 10; // expose

	float blockedMultiplier = 0.80f;
	float glancedMultiplier = 0.60f;

    bool allVulnerabilities = false; // if true, assumes total vuln all the time

	EnemyInfo() 
	{
		stats.blockRating = 450; // Eidolon NM
		// dummy is 100 block

        stats.defenceRating = 360; // -> 650 hit  = 0 glance
		// dummy is 100 defense
		// TODO: test if really 100 or offset

		// TODO: check evade rating
	}
};
