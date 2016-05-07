#pragma once

#include <cassert>

#include "common.hh"

#include "Weapon.hh"
#include "Types.hh"

struct Skill
{
    string name;
    Weapon weapon;
    int timeIn60th = 0; ///< 60 = 1s, 150 = 2.5s, 0 = no GCD
    int hits = 0;       ///< 0 = not damaging ability
    float dmgScaling;   ///< dmgScaling * combat power = base dmg
    float dmgScaling5;  ///< scaling at 5 resources
    SubType subtype = SubType::None;
    DmgType dmgtype;
    SkillType skilltype;

    // 1.0  for no penalty
    // ??   for 2 hits per sec
    // 0.8  for 3 hits per sec
    // 0.75 for 4 hits per sec
    // ??   for 5 hits per sec
    float multiHitPenalty() const {
        assert(hits > 0 && "only for damaging abilities");
        switch (timeIn60th / hits) {
            case 60: return 1.0f;
            case 30: return 0.85f; // ???
            case 20: return 0.8f;
            case 15: return 0.75f;
            case 12: return 0.7f; // ???
            default:
                assert(0 && "data missing");
                return -1;
        }
    }
};
