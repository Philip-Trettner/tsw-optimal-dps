#pragma once

#include "common.hh"

#include "Skillset.hh"
#include "Gear.hh"
#include "Rotation.hh"

struct Build
{
    Skillset skills;
    Gear gear;
    SharedRotation rotation;
    Stats potionStats;

    void shortDump() const;

    jsonxx::Object toJson() const;
    void fromJson(jsonxx::Object const& o);
};

// custom specialization of std::hash can be injected in namespace std
namespace std
{
template <>
struct hash<Build>
{
    size_t operator()(Build const& s) const
    {
        size_t h = 0x851bc2;

        hash_combine(h, s.skills);
        hash_combine(h, s.gear);

        return h;
    }
};
}

inline bool operator==(Build const& l, Build const& r)
{
    return l.skills == r.skills && l.gear == r.gear;
}
inline bool operator!=(Build const& l, Build const& r)
{
    return !(l == r);
}
