#pragma once

struct EnemyInfo;

struct Stats
{
    // ratings
    int health = 0;
    int attackRating = 0;
    int healRating = 0;

    int weaponPower = 0;

    int hitRating = 0;
    int critRating = 0;
    int critPowerRating = 0;
    int penRating = 0;

    int blockRating = 0;
    int physProtRating = 0;
    int magProtRating = 0;
    int evadeRating = 0;
    int defenceRating = 0;

    // flat perc increases
    // .2 = 20%
    float addedCritChance = 0;
    float addedCritPower = 0;
    float addedPenChance = 0;

    // damage inc (.3 = 30%)
    float additiveDamage = 0;       // "Bracket A"
    float multiplicativeDamage = 0; // "Bracket B"

    // final values
    // call update() to refresh these
    float finalCombatPower = -1;
    float finalCritChance = -1;
    float finalCritPower = -1;
    float finalPenChance = -1;
    float finalDmgMultiplier = -1; // is > 1 (e.g. 1.5 for 50% additive dmg)

    void update(EnemyInfo const& enemy);

};

inline Stats operator+(Stats const& l, Stats const& r)
{
    Stats s;
    s.health = l.health + r.health;

    // TODO: continue
    return s;
}
