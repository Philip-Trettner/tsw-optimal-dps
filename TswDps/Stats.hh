#pragma once

#include <functional>

struct EnemyInfo;

enum class Rating
{
    Hit,
    Crit,
    CritPower,
    Pen,

    Block,
    PhysProt,
    MagProt,
    Evade,
    Defence
};

enum class PrimaryStat
{
    Attack,
    Heal,
    HP
};

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

    /// dumps hit, crit, crit power, pen
    void dumpDpsGlyphs() const;
    /// dumps ar, hit, crit, crit power, pen
    void dumpDpsStats(bool updateWithEmpty = true);

    /// returns AR, HR, HP, WP part
    Stats getPrimaryPart() const
    {
        Stats s;
        s.attackRating = attackRating;
        s.healRating = healRating;
        s.health = health;
        s.weaponPower = weaponPower;
        return s;
    }
};

// custom specialization of std::hash can be injected in namespace std
namespace std
{
template <>
struct hash<Stats>
{
    template <typename T>
    void hash_combine(size_t& seed, T const& v) const
    {
        seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    typedef Stats argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& s) const
    {
        std::size_t h = 0x851bc1;

        hash_combine(h, s.health);
        hash_combine(h, s.attackRating);
        hash_combine(h, s.healRating);

        hash_combine(h, s.weaponPower);

        hash_combine(h, s.hitRating);
        hash_combine(h, s.critRating);
        hash_combine(h, s.critPowerRating);
        hash_combine(h, s.penRating);

        hash_combine(h, s.blockRating);
        hash_combine(h, s.physProtRating);
        hash_combine(h, s.magProtRating);
        hash_combine(h, s.evadeRating);
        hash_combine(h, s.defenceRating);

        hash_combine(h, s.addedCritChance);
        hash_combine(h, s.addedCritPower);
        hash_combine(h, s.addedPenChance);

        hash_combine(h, s.additiveDamage);
        hash_combine(h, s.multiplicativeDamage);

        return h;
    }
};
}

inline Stats operator+(Stats const& l, Stats const& r)
{
    Stats s;
    s.health = l.health + r.health;
    s.attackRating = l.attackRating + r.attackRating;
    s.healRating = l.healRating + r.healRating;

    s.weaponPower = l.weaponPower + r.weaponPower;

    s.hitRating = l.hitRating + r.hitRating;
    s.critRating = l.critRating + r.critRating;
    s.critPowerRating = l.critPowerRating + r.critPowerRating;
    s.penRating = l.penRating + r.penRating;

    s.blockRating = l.blockRating + r.blockRating;
    s.physProtRating = l.physProtRating + r.physProtRating;
    s.magProtRating = l.magProtRating + r.magProtRating;
    s.evadeRating = l.evadeRating + r.evadeRating;
    s.defenceRating = l.defenceRating + r.defenceRating;

    s.addedCritChance = l.addedCritChance + r.addedCritChance;
    s.addedCritPower = l.addedCritPower + r.addedCritPower;
    s.addedPenChance = l.addedPenChance + r.addedPenChance;

    s.additiveDamage = l.additiveDamage + r.additiveDamage;
    s.multiplicativeDamage = l.multiplicativeDamage + r.multiplicativeDamage;

    return s;
}

inline Stats operator*(Stats const& l, float f)
{
    Stats s;
    s.health = l.health * f;
    s.attackRating = l.attackRating * f;
    s.healRating = l.healRating * f;

    s.weaponPower = l.weaponPower * f;

    s.hitRating = l.hitRating * f;
    s.critRating = l.critRating * f;
    s.critPowerRating = l.critPowerRating * f;
    s.penRating = l.penRating * f;

    s.blockRating = l.blockRating * f;
    s.physProtRating = l.physProtRating * f;
    s.magProtRating = l.magProtRating * f;
    s.evadeRating = l.evadeRating * f;
    s.defenceRating = l.defenceRating * f;

    s.addedCritChance = l.addedCritChance * f;
    s.addedCritPower = l.addedCritPower * f;
    s.addedPenChance = l.addedPenChance * f;

    s.additiveDamage = l.additiveDamage * f;
    s.multiplicativeDamage = l.multiplicativeDamage * f;

    return s;
}

inline bool operator==(Stats const& l, Stats const& r)
{
    if (l.health != r.health)
        return false;
    if (l.attackRating != r.attackRating)
        return false;
    if (l.healRating != r.healRating)
        return false;

    if (l.weaponPower != r.weaponPower)
        return false;

    if (l.hitRating != r.hitRating)
        return false;
    if (l.critRating != r.critRating)
        return false;
    if (l.critPowerRating != r.critPowerRating)
        return false;
    if (l.penRating != r.penRating)
        return false;

    if (l.blockRating != r.blockRating)
        return false;
    if (l.physProtRating != r.physProtRating)
        return false;
    if (l.magProtRating != r.magProtRating)
        return false;
    if (l.evadeRating != r.evadeRating)
        return false;
    if (l.defenceRating != r.defenceRating)
        return false;

    if (l.addedCritChance != r.addedCritChance)
        return false;
    if (l.addedCritPower != r.addedCritPower)
        return false;
    if (l.addedPenChance != r.addedPenChance)
        return false;

    if (l.additiveDamage != r.additiveDamage)
        return false;
    if (l.multiplicativeDamage != r.multiplicativeDamage)
        return false;

    return true;
}

inline bool operator!=(Stats const& l, Stats const& r)
{
    return !(l == r);
}
