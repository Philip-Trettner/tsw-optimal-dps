#pragma once

#include <array>
#include <vector>

#include "Signet.hh"
#include "Stats.hh"

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
        void fix(Rating r);
        /// fixes this piece to a 50-50 stat split
        void fix(Rating r1, Rating r2);
        /// sets the inital rating but makes this a free piece
        void free(Rating r);
        /// sets the inital rating but makes this a free piece
        void free(Rating r1, Rating r2);

        /// sets the primary stat of this talisman
        void set(PrimaryStat stat, TalismanQuality q);
    };

    // all gear pieces except aux
    // head -> major -> minor -> weapon
    std::array<Piece, 1 + 3 + 3 + 2> pieces;

    /// Left hand weapon
    Weapon leftWeapon;
    /// Right hand weapon
    Weapon rightWeapon;

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
    static std::vector<Stats> enumeratePieceStats(Slot slot, std::vector<Rating> const& ratings, bool allowSplits = true);

    /// returns a 100% stat for a given rating
    static Stats singleStatOf(Slot slot, Rating r);
    /// returns a 50% stat for a given rating
    static Stats splitStatOf(Slot slot, Rating r);
};
