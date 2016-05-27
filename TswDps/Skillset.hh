#pragma once

#include "common.hh"
#include "Skill.hh"
#include "Augment.hh"
#include "Passive.hh"

struct Skillset
{
    Skill skills[SKILL_CNT];
    Augment augments[SKILL_CNT];
    std::vector<Passive> passives;
};

// custom specialization of std::hash can be injected in namespace std
namespace std
{
template <>
struct hash<Skillset>
{
    size_t operator()(Skillset const& ss) const
    {
        size_t h = 0x851bc3;

        for (auto const& s : ss.skills)
            hash_combine(h, s.name);
        for (auto const& s : ss.augments)
            hash_combine(h, s.name);
        for (auto const& s : ss.passives)
            hash_combine(h, s.name);

        return h;
    }
};
}

inline bool operator==(Skillset const& l, Skillset const& r)
{
    for (auto i = 0; i < SKILL_CNT; ++i)
        if (l.skills[i].name != r.skills[i].name)
            return false;
    for (auto i = 0; i < SKILL_CNT; ++i)
        if (l.augments[i].name != r.augments[i].name)
            return false;

    if (l.passives.size() != r.passives.size())
        return false;

    for (auto i = 0u; i < l.passives.size(); ++i)
        if (l.passives[i].name != r.passives[i].name)
            return false;

    return true;
}
inline bool operator!=(Skillset const& l, Skillset const& r)
{
    return !(l == r);
}
