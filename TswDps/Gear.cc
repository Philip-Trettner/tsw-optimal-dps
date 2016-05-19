#include "Gear.hh"

#include <cassert>

#include <unordered_set>

Gear::Gear()
{
    for (auto i = 0u; i < pieces.size(); ++i)
        pieces[i].status = SlotStatus::Free;

    pieces[Head].slot = Slot::Head;

    pieces[MajorLeft].slot = Slot::Major;
    pieces[MajorMid].slot = Slot::Major;
    pieces[MajorRight].slot = Slot::Major;

    pieces[MinorLeft].slot = Slot::Minor;
    pieces[MinorMid].slot = Slot::Minor;
    pieces[MinorRight].slot = Slot::Minor;

    pieces[WeaponLeft].slot = Slot::Weapon;
    pieces[WeaponRight].slot = Slot::Weapon;
}

void Gear::setGear(PrimaryStat stat, Gear::TalismanQuality q)
{
    for (auto &p : pieces)
        p.set(stat, q);
}

Stats Gear::gearStats() const
{
    Stats s;
    for (auto const &p : pieces)
        if (p.slot != Slot::Weapon)
            s = s + p.stats;
    return s;
}

std::vector<Stats> Gear::enumerateGearStats(const std::vector<Rating> &ratings, bool allowSplits) const
{
    assert(ratings.size() > 0 && "specify at least one rating");

    auto statsHead = enumeratePieceStats(Slot::Head, ratings, allowSplits);
    auto statsMajor = enumeratePieceStats(Slot::Major, ratings, allowSplits);
    auto statsMinor = enumeratePieceStats(Slot::Minor, ratings, allowSplits);

    std::array<std::unordered_set<Stats>, 1 + 1 + 3 + 3> stats;

    stats[0].insert(Stats());

    for (auto i = 0; i < 1 + 3 + 3; ++i)
    {
        auto const &currStats = stats[i];
        auto &nextStats = stats[i + 1];

        // fixed piece
        switch (pieces[i].status)
        {
        case SlotStatus::Fixed:
            for (auto const &s : currStats)
                nextStats.insert(s + pieces[i].stats);
            break;
        case SlotStatus::Free:
        {
            auto const &statsOptions
                = pieces[i].slot == Slot::Head ? statsHead : pieces[i].slot == Slot::Major ? statsMajor : statsMinor;
            for (auto const &s : currStats)
                for (auto const &s2 : statsOptions)
                    nextStats.insert(s + s2);
        }
        break;
        default:
            assert(0);
        }
    }

    return std::vector<Stats>(begin(stats.back()), end(stats.back()));
}

std::vector<Stats> Gear::enumeratePieceStats(Slot slot, const std::vector<Rating> &ratings, bool allowSplits)
{
    assert(ratings.size() > 0 && "specify at least one rating");
    std::vector<Stats> stats;

    // single stats
    for (auto r : ratings)
        stats.push_back(singleStatOf(slot, r));

    // double stats
    if (allowSplits)
        for (auto i1 = 0u; i1 < ratings.size(); ++i1)
            for (auto i2 = i1 + 1; i2 < ratings.size(); ++i2)
                stats.push_back(splitStatOf(slot, ratings[i1]) + splitStatOf(slot, ratings[i2]));

    return stats;
}

Stats Gear::singleStatOf(Gear::Slot slot, Rating r)
{
    Stats s;
    switch (slot)
    {
    case Slot::Weapon:
    case Slot::Head:
        switch (r)
        {
        case Rating::Hit:
            s.hitRating = 407;
            break;
        case Rating::Pen:
            s.penRating = 407;
            break;
        case Rating::Crit:
            s.critRating = 362;
            break;
        case Rating::CritPower:
            s.critPowerRating = 347;
            break;

        case Rating::Block:
            s.blockRating = 407;
            break;
        case Rating::Defence:
            s.defenceRating = 407;
            break;
        case Rating::Evade:
            s.evadeRating = 367;
            break;
        case Rating::PhysProt:
            s.physProtRating = 362;
            break;
        case Rating::MagProt:
            s.magProtRating = 362;
            break;
        default:
            assert(0);
            break;
        }
        break;
    case Slot::Major:
        switch (r)
        {
        case Rating::Hit:
            s.hitRating = 367;
            break;
        case Rating::Pen:
            s.penRating = 367;
            break;
        case Rating::Crit:
            s.critRating = 327;
            break;
        case Rating::CritPower:
            s.critPowerRating = 313;
            break;

        case Rating::Block:
            s.blockRating = 367;
            break;
        case Rating::Defence:
            s.defenceRating = 367;
            break;
        case Rating::Evade:
            s.evadeRating = 332;
            break;
        case Rating::PhysProt:
            s.physProtRating = 327;
            break;
        case Rating::MagProt:
            s.magProtRating = 327;
            break;
        default:
            assert(0);
            break;
        }
        break;
    case Slot::Minor:
        switch (r)
        {
        case Rating::Hit:
            s.hitRating = 236;
            break;
        case Rating::Pen:
            s.penRating = 236;
            break;
        case Rating::Crit:
            s.critRating = 210;
            break;
        case Rating::CritPower:
            s.critPowerRating = 201;
            break;

        case Rating::Block:
            s.blockRating = 236;
            break;
        case Rating::Defence:
            s.defenceRating = 236;
            break;
        case Rating::Evade:
            s.evadeRating = 213;
            break;
        case Rating::PhysProt:
            s.physProtRating = 210;
            break;
        case Rating::MagProt:
            s.magProtRating = 210;
            break;
        default:
            assert(0);
            break;
        }
        break;
    default:
        assert(0);
        break;
    }
    return s;
}

Stats Gear::splitStatOf(Gear::Slot slot, Rating r)
{
    return singleStatOf(slot, r) * 0.5f;
}

void Gear::Piece::fix(Rating r)
{
    status = SlotStatus::Fixed;
    auto s = stats.getPrimaryPart();
    stats = singleStatOf(slot, r) + s;
}

void Gear::Piece::fix(Rating r1, Rating r2)
{
    status = SlotStatus::Fixed;
    auto s = stats.getPrimaryPart();
    stats = splitStatOf(slot, r1) + splitStatOf(slot, r2) + s;
}

void Gear::Piece::free(Rating r)
{
    status = SlotStatus::Free;
    auto s = stats.getPrimaryPart();
    stats = singleStatOf(slot, r) + s;
}

void Gear::Piece::free(Rating r1, Rating r2)
{
    status = SlotStatus::Free;
    auto s = stats.getPrimaryPart();
    stats = splitStatOf(slot, r1) + splitStatOf(slot, r2) + s;
}

void Gear::Piece::set(PrimaryStat stat, TalismanQuality q)
{
    stats.attackRating = 0;
    stats.healRating = 0;
    stats.health = 0;

    int val = 0;

    switch (q)
    {
    case TalismanQuality::QL10_9:
        switch (slot)
        {
        case Slot::Head:
            switch (stat)
            {
            case PrimaryStat::Attack:
            case PrimaryStat::Heal:
                val = 1077;
                break;
            case PrimaryStat::HP:
                val = 3021;
                break;
            default:
                assert(0);
            }
            break;
        case Slot::Major:
            switch (stat)
            {
            case PrimaryStat::Attack:
            case PrimaryStat::Heal:
                val = 972;
                break;
            case PrimaryStat::HP:
                val = 2728;
                break;
            default:
                assert(0);
            }
            break;
        case Slot::Minor:
            switch (stat)
            {
            case PrimaryStat::Attack:
            case PrimaryStat::Heal:
                val = 625;
                break;
            case PrimaryStat::HP:
                val = 1754;
                break;
            default:
                assert(0);
            }
            break;
        case Slot::Weapon:
            val = 510;
            break;
        default:
            assert(0);
        }
        break;

    case TalismanQuality::QL11:
        switch (slot)
        {
        case Slot::Head:
            switch (stat)
            {
            case PrimaryStat::Attack:
            case PrimaryStat::Heal:
                val = 1144;
                break;
            case PrimaryStat::HP:
                val = 3186;
                break;
            default:
                assert(0);
            }
            break;
        case Slot::Major:
            switch (stat)
            {
            case PrimaryStat::Attack:
            case PrimaryStat::Heal:
                val = 1033;
                break;
            case PrimaryStat::HP:
                val = 2877;
                break;
            default:
                assert(0);
            }
            break;
        case Slot::Minor:
            switch (stat)
            {
            case PrimaryStat::Attack:
            case PrimaryStat::Heal:
                val = 664;
                break;
            case PrimaryStat::HP:
                val = 1850;
                break;
            default:
                assert(0);
            }
            break;
        case Slot::Weapon:
            val = 528;
            break;
        default:
            assert(0);
        }
        break;

    default:
        assert(0);
    }

    if (slot == Slot::Weapon)
        stats.weaponPower = val;
    else
        switch (stat)
        {
        case PrimaryStat::Attack:
            stats.attackRating = val;
            break;
        case PrimaryStat::Heal:
            stats.healRating = val;
            break;
        case PrimaryStat::HP:
            stats.health = val;
            break;
        }
}
