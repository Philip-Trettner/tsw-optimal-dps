#pragma once

#include <array>
#include <vector>

#include "Signet.hh"
#include "Stats.hh"

#include "jsonxx.hh"

struct Gear
{
    static const size_t Head = 0;
    static const size_t MajorLeft = 1;
    static const size_t MajorMid = 2;
    static const size_t MajorRight = 3;
    static const size_t MinorLeft = 4;
    static const size_t MinorMid = 5;
    static const size_t MinorRight = 6;
    static const size_t WeaponLeft = 7;
    static const size_t WeaponRight = 8;

    enum class SlotStatus
    {
        Free,
        Fixed
    };

    enum class Slot
    {
        Head,
        Major,
        Minor,
        Weapon
    };

    enum class TalismanQuality
    {
        QL10_4,
        QL10_9,
        QL11
    };

    struct Piece
    {
        SlotStatus status;
        Slot slot;
        Stats stats;
        Signet signet;

        /// fixes this piece to a single stat
        void fix(Rating r, bool ql11 = true);
        /// fixes this piece to a 50-50 stat split
        void fix(Rating r1, Rating r2, bool ql11 = true);
        /// sets the inital rating but makes this a free piece
        void free(Rating r, bool ql11 = true);
        /// sets the inital rating but makes this a free piece
        void free(Rating r1, Rating r2, bool ql11 = true);

        /// sets the primary stat of this talisman
        void set(PrimaryStat stat, TalismanQuality q);

        jsonxx::Object toJson() const;
        void fromJson(jsonxx::Object const& o);
    };

    // all gear pieces except aux
    // head -> major -> minor -> weapon
    std::array<Piece, 1 + 3 + 3 + 2> pieces;

    /// Left hand weapon
    Weapon leftWeapon;
    /// Right hand weapon
    Weapon rightWeapon;

    /// Effect for stimulant
    EffectSlot stimulant = EffectSlot::Count;
    /// Passive for kickback
    Passive kickback;

    /// default dps gear (for now)
    void loadStandardDpsGear();
    /// empty dps gear
    void loadEmptyDpsGear();

    /// sets both weapons
    void setWeapons(Weapon left, Weapon right);

    std::string pieceName(int slot)
    {
        switch (slot)
        {
        case Gear::Head:
            return "Head";
        case Gear::MajorLeft:
            return "Major Left";
        case Gear::MajorMid:
            return "Major Mid";
        case Gear::MajorRight:
            return "Major Right";
        case Gear::MinorLeft:
            return "Minor Left";
        case Gear::MinorMid:
            return "Minor Mid";
        case Gear::MinorRight:
            return "Minor Right";
        case Gear::WeaponLeft:
            return to_string(leftWeapon);
        case Gear::WeaponRight:
            return to_string(rightWeapon);
        default:
            assert(0);
            return "";
        }
    }

    Gear();

    /// sets all gear pieces at once (includes weapons)
    void setGear(PrimaryStat stat, TalismanQuality q);

    /// Returns stat sum without weapons
    Stats gearStats() const;

    /// enumerates all possible stat distributions for the given ratings
    /// (acknowledges fixed pieces)
    /// does NOT include weapons
    /// assumes QL11
    /// (each return value is unique)
    std::vector<Stats> enumerateGearStats(std::vector<Rating> const& ratings, bool allowSplits = true) const;

    /// returns all possible stats combinations of a single slot
    /// assumes QL11
    /// (each return value is unique)
    static std::vector<Stats> enumeratePieceStats(Slot slot, std::vector<Rating> const& ratings, bool ql11, bool allowSplits = true);

    /// returns a 100% stat for a given rating
    static Stats singleStatOf(Slot slot, Rating r, bool ql11);
    /// returns a 50% stat for a given rating
    static Stats splitStatOf(Slot slot, Rating r, bool ql11);

    /// sets neck to QL11 + violence
    void setNeckQL11();
    /// sets neck to woodcutters
    void setNeckWoodcutters();
    /// sets neck to 10.9 egon
    void setNeckEgon();

    /// sets finger to QL11 + violence
    void setFingerQL11();
    /// sets finger to Coney
    void setFingerConey();
};

// custom specialization of std::hash can be injected in namespace std
namespace std
{
template <>
struct hash<Gear>
{
    size_t operator()(Gear const& g) const
    {
        size_t h = 0x851bc4;

        for (auto const& p : g.pieces)
        {
            hash_combine(h, p.signet.name());
            hash_combine(h, p.stats);
        }

        return h;
    }
};
}

inline bool operator==(Gear const& l, Gear const& r)
{
    for (auto i = Gear::Head; i <= Gear::WeaponRight; ++i)
    {
        auto const& pl = l.pieces[i];
        auto const& pr = r.pieces[i];

        if (pl.signet.name() != pr.signet.name())
            return false;
        if (pl.stats != pr.stats)
            return false;
    }

    return true;
}
inline bool operator!=(Gear const& l, Gear const& r)
{
    return !(l == r);
}
