#pragma once

#include <cassert>

#include "common.hh"

#include "Types.hh"
#include "Weapon.hh"
#include "Passive.hh"

struct Skill
{
    string name;

    Weapon weapon = Weapon::None;
    SubType subtype = SubType::None;
    DmgType dmgtype = DmgType::None;
    SkillType skilltype = SkillType::None;

    int timeIn60th = 0;      ///< 60 = 1s, 150 = 2.5s, 0 = no GCD
    int casttimeIn60th = 0;  ///< 0 = no casttime, casttime and CD are parallel
    int cooldownIn60th = 0;  ///< 0 = no CD
    int hits = 0;            ///< 0 = not damaging ability
    bool channeling = false; ///< if true, hits are channeled equally over time

    float dmgScaling;       ///< dmgScaling * combat power = base dmg (at 1 resource for consumers)
    float dmgScaling5 = -1; ///< scaling at 5 resources
    float dmgScaling0 = -1; ///< if above 0, scaling at 0 resources

    bool consumesAnyways = false; ///< if true, this skill consumes resources even if not a consumer (for bullet ballet)

    // dmg @ low HP
    float dmgScalingLow = -1;
    float dmgScaling5Low = -1;

    int specialHitsA = 0;
    float dmgScalingA = -1;
    int specialHitsB = 0;
    float dmgScalingB = -1;
    int specialHitsC = 0;
    float dmgScalingC = -1;

    int extraHitPerResource = 0;    ///< extra hits per resource, NOT FULL HITS
    float fixedMultiHitPenalty = 0; ///< if above zero, replaces MH penalty

    float chanceForScaleInc = 0; ///< chance to increase base scaling
    float scaleIncPerc = 0;      ///< amount of base scale inc if proc

    float baseDmgIncPerHit = 0; ///< % inc per hit of base scaling, _additive_

    int fixedConsumerResources = 0; ///< 0 = consumes all and uses dmgScaling5

    bool buildPrimaryOnly = false; ///< if true and builder, only builds for current weapon

    bool animaDeviation = false;
    DmgType appliesVulnerability = DmgType::None; ///< if non-None, applies EVS

    bool slotForDmgAug = true;      ///< if true, this skill may slot a dmg augment
    bool slotForSupportAug = false; ///< if true, this skill may slot a support augment

    int reduceWeaponConsumerCD = 0; ///< if above 0, reduces CD of same-weapon consumers (after activation)

    Passive passive; ///< built-in passive

    // 1.0  for no penalty
    // ??   for 2 hits per sec
    // 0.8  for 3 hits per sec
    // 0.75 for 4 hits per sec
    // ??   for 5 hits per sec
    float multiHitPenalty() const
    {
        assert(hits > 0 && "only for damaging abilities");

        if (fixedMultiHitPenalty > 0)
            return fixedMultiHitPenalty;

        switch (timeIn60th / hits)
        {
        case 120:
        case 90:
        case 60:
            return 1.0f;
        case 30:
            return 0.85f; // ???
        case 20:
            return 0.8f; // ???
        case 18:
            return 0.725f;
        case 15:
            return 0.75f;
        case 12:
            return 0.7f; // ???
        default:
            assert(0 && "data missing");
            return -1;
        }
    }
};
