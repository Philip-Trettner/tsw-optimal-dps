#pragma once

#include <functional>
#include "common.hh"
#include "jsonxx.hh"

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

inline string to_string(Rating r)
{
    switch (r)
    {
    case Rating::Hit:
        return "Hit";
    case Rating::Crit:
        return "Crit";
    case Rating::CritPower:
        return "CritPower";
    case Rating::Pen:
        return "Pen";
    default:
        assert(0 && "not impl");
        return "";
    }
}

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

    // special stuff
    bool egonPendant = false;

    // flat perc increases
    // .2 = 20%
    float addedCritChance = 0;
    float addedCritPower = 0;
    float addedPenChance = 0;
    float reducedGlanceChance = 0;

    // damage inc (.3 = 30%)
    float additiveDamage = 0;       // "Bracket A"
    float multiplicativeDamage = 0; // "Bracket B"

    // final values
    // call update() to refresh these
    float finalCombatPower = -1;
    float finalCritChance = -1;
    float finalCritPower = -1;
    float finalPenChance = -1;
    float finalBlockChance = -1;
    float finalGlanceChance = -1;
    float finalEvadeChance = -1;
    float finalDmgMultiplier = -1; // is > 1 (e.g. 1.5 for 50% additive dmg)

    void update(EnemyInfo const& enemy);

    /// dumps hit, crit, crit power, pen
    void dumpDpsGlyphs() const;
    /// dumps ar, hit, crit, crit power, pen
    void dumpDpsStats(bool updateWithEmpty = true);

    jsonxx::Object toJson() const
    {
        jsonxx::Object o;
        if (attackRating > 0)
            o << "AR" << attackRating;
        if (hitRating > 0)
            o << "Hit" << hitRating;
        if (penRating > 0)
            o << "Pen" << penRating;
        if (critRating > 0)
            o << "Crit" << critRating;
        if (critPowerRating > 0)
            o << "CritPower" << critPowerRating;
        // TODO: more
        return o;
    }

    void fromJson(jsonxx::Object const& o)
    {
        // only overwrites present members
        using namespace jsonxx;
        if (o.has<Number>("AR"))
            attackRating = (int)o.get<Number>("AR");
        if (o.has<Number>("Hit"))
            hitRating = (int)o.get<Number>("Hit");
        if (o.has<Number>("Pen"))
            penRating = (int)o.get<Number>("Pen");
        if (o.has<Number>("Crit"))
            critRating = (int)o.get<Number>("Crit");
        if (o.has<Number>("CritPower"))
            critPowerRating = (int)o.get<Number>("CritPower");
    }

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

    /// sets a given rating explicitly
    void set(Rating r, int value)
    {
        switch (r)
        {
        case Rating::Hit:
            hitRating = value;
            break;
        case Rating::Crit:
            critRating = value;
            break;
        case Rating::CritPower:
            critPowerRating = value;
            break;
        case Rating::Pen:
            penRating = value;
            break;
        default:
            assert(0 && "not impl");
            break;
        }
    }
};

// custom specialization of std::hash can be injected in namespace std
namespace std
{
template <>
struct hash<Stats>
{
    size_t operator()(Stats const& s) const
    {
        size_t h = 0x851bc1;

        hash_combine(h, s.health);
        hash_combine(h, s.attackRating);
        hash_combine(h, s.healRating);

        hash_combine(h, s.weaponPower);

        hash_combine(h, s.egonPendant);

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
        hash_combine(h, s.reducedGlanceChance);

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
    s.reducedGlanceChance = l.reducedGlanceChance + r.reducedGlanceChance;

    s.additiveDamage = l.additiveDamage + r.additiveDamage;
    s.multiplicativeDamage = l.multiplicativeDamage + r.multiplicativeDamage;

    s.egonPendant = l.egonPendant || r.egonPendant;

    return s;
}

inline Stats operator*(Stats const& l, float f)
{
    Stats s;
    s.health = static_cast<int>(l.health * f);
    s.attackRating = static_cast<int>(l.attackRating * f);
    s.healRating = static_cast<int>(l.healRating * f);

    s.weaponPower = static_cast<int>(l.weaponPower * f);

    s.hitRating = static_cast<int>(l.hitRating * f);
    s.critRating = static_cast<int>(l.critRating * f);
    s.critPowerRating = static_cast<int>(l.critPowerRating * f);
    s.penRating = static_cast<int>(l.penRating * f);

    s.blockRating = static_cast<int>(l.blockRating * f);
    s.physProtRating = static_cast<int>(l.physProtRating * f);
    s.magProtRating = static_cast<int>(l.magProtRating * f);
    s.evadeRating = static_cast<int>(l.evadeRating * f);
    s.defenceRating = static_cast<int>(l.defenceRating * f);

    s.addedCritChance = l.addedCritChance * f;
    s.addedCritPower = l.addedCritPower * f;
    s.addedPenChance = l.addedPenChance * f;
    s.reducedGlanceChance = l.reducedGlanceChance * f;

    s.additiveDamage = l.additiveDamage * f;
    s.multiplicativeDamage = l.multiplicativeDamage * f;

    s.egonPendant = l.egonPendant;

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

    if (l.egonPendant != r.egonPendant)
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
    if (l.reducedGlanceChance != r.reducedGlanceChance)
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

inline string shortStatDump(Stats const& s, bool withBase = true, bool withBB = false)
{
    auto prefix = withBB ? "[color=#FFEB3B][b]" : "";
    auto suffix = withBB ? "[/b][/color]" : "";

    string ss;
    if (s.attackRating > 0 && withBase)
        ss += (ss.empty() ? "" : ", ") + std::to_string(s.attackRating) + (withBB ? "[color=#D32F2F][b]" : "") + " AR" + suffix;
    if (s.hitRating > 0)
        ss += (ss.empty() ? "" : ", ") + std::to_string(s.hitRating) + prefix + " Hit" + suffix;
    if (s.penRating > 0)
        ss += (ss.empty() ? "" : ", ") + std::to_string(s.penRating) + prefix + " Pen" + suffix;
    if (s.critRating > 0)
        ss += (ss.empty() ? "" : ", ") + std::to_string(s.critRating) + prefix + " Crit" + suffix;
    if (s.critPowerRating > 0)
        ss += (ss.empty() ? "" : ", ") + std::to_string(s.critPowerRating) + prefix + " CritPower" + suffix;
    return ss;
}