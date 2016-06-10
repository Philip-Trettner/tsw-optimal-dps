#include "Gear.hh"

#include <cassert>

#include "Signets.hh"
#include "Passives.hh"

#include <unordered_set>

void Gear::loadStandardDpsGear()
{
    setGear(PrimaryStat::Attack, TalismanQuality::QL11);

    // Head
    pieces[Head].free(Rating::Hit);
    pieces[Head].signet = Signets::HeadWeapon::Laceration();

    // Finger
    pieces[MajorLeft].free(Rating::Crit);
    pieces[MajorLeft].signet = Signets::Major::Violence();

    // Neck (WC)
    // pieces[MajorMid].set(PrimaryStat::Attack, TalismanQuality::QL10_9);
    pieces[MajorMid].free(Rating::CritPower);
    // pieces[MajorMid].signet = Signets::Major::WoodcuttersWrath();
    // pieces[MajorMid].signet = Signets::Major::EgonPendant();
    pieces[MajorMid].signet = Signets::Major::Violence();

    // Wrist
    pieces[MajorRight].free(Rating::Pen);
    pieces[MajorRight].signet = Signets::Major::Violence();

    // Luck
    pieces[MinorLeft].set(PrimaryStat::Attack, TalismanQuality::QL10_9);
    pieces[MinorLeft].free(Rating::Hit);
    pieces[MinorLeft].signet = Signets::Minor::SubwayTokens();

    // Waist
    pieces[MinorMid].free(Rating::Crit);
    pieces[MinorMid].signet = Signets::Minor::Issue1p5();

    // Occult
    pieces[MinorRight].free(Rating::Crit);
    pieces[MinorRight].signet = Signets::Minor::Issue1p5();

    // Weapons
    pieces[WeaponLeft].free(Rating::Crit);
    pieces[WeaponLeft].signet = Signets::HeadWeapon::Aggression();

    pieces[WeaponRight].free(Rating::Crit);
    pieces[WeaponRight].signet = Signets::HeadWeapon::Aggression();

    stimulant = EffectSlot::StimAttackPurple;
    kickback = Passives::Kickback::CritPowerOnCritPurple();
}

void Gear::loadEmptyDpsGear()
{
    setGear(PrimaryStat::Attack, TalismanQuality::QL11);

    // violences currently don't get optimized except neck
    pieces[MajorLeft].signet = Signets::Major::Violence();
    pieces[MajorMid].signet = Signets::Major::Violence();
    pieces[MajorRight].signet = Signets::Major::Violence();
}

void Gear::setWeapons(Weapon left, Weapon right)
{
    leftWeapon = left;
    rightWeapon = right;
}

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

std::vector<Stats> Gear::enumeratePieceStats(Slot slot, const std::vector<Rating> &ratings, bool ql11, bool allowSplits)
{
    assert(ratings.size() > 0 && "specify at least one rating");
    std::vector<Stats> stats;

    // single stats
    for (auto r : ratings)
        stats.push_back(singleStatOf(slot, r, ql11));

    // double stats
    if (allowSplits)
        for (auto i1 = 0u; i1 < ratings.size(); ++i1)
            for (auto i2 = i1 + 1; i2 < ratings.size(); ++i2)
                stats.push_back(splitStatOf(slot, ratings[i1], ql11) + splitStatOf(slot, ratings[i2], ql11));

    return stats;
}

Stats Gear::singleStatOf(Gear::Slot slot, Rating r, bool ql11)
{
    Stats s;
    switch (slot)
    {
    case Slot::Weapon:
    case Slot::Head:
        switch (r)
        {
        case Rating::Hit:
            s.hitRating = !ql11 ? 363 : 407;
            break;
        case Rating::Pen:
            s.penRating = !ql11 ? 363 : 407;
            break;
        case Rating::Crit:
            s.critRating = !ql11 ? 336 : 362;
            break;
        case Rating::CritPower:
            s.critPowerRating = !ql11 ? 328 : 347;
            break;

        case Rating::Block:
            s.blockRating = !ql11 ? 363 : 407;
            break;
        case Rating::Defence:
            s.defenceRating = !ql11 ? 363 : 407;
            break;
        case Rating::Evade:
            s.evadeRating = !ql11 ? 337 : 367;
            break;
        case Rating::PhysProt:
            s.physProtRating = !ql11 ? 336 : 362;
            break;
        case Rating::MagProt:
            s.magProtRating = !ql11 ? 336 : 362;
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
            s.hitRating = !ql11 ? 328 : 367;
            break;
        case Rating::Pen:
            s.penRating = !ql11 ? 328 : 367;
            break;
        case Rating::Crit:
            s.critRating = !ql11 ? 303 : 327;
            break;
        case Rating::CritPower:
            s.critPowerRating = !ql11 ? 296 : 313;
            break;

        case Rating::Block:
            s.blockRating = !ql11 ? 328 : 367;
            break;
        case Rating::Defence:
            s.defenceRating = !ql11 ? 328 : 367;
            break;
        case Rating::Evade:
            s.evadeRating = !ql11 ? 304 : 332;
            break;
        case Rating::PhysProt:
            s.physProtRating = !ql11 ? 304 : 327;
            break;
        case Rating::MagProt:
            s.magProtRating = !ql11 ? 304 : 327;
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
            s.hitRating = !ql11 ? 211 : 236;
            break;
        case Rating::Pen:
            s.penRating = !ql11 ? 211 : 236;
            break;
        case Rating::Crit:
            s.critRating = !ql11 ? 195 : 210;
            break;
        case Rating::CritPower:
            s.critPowerRating = !ql11 ? 191 : 201;
            break;

        case Rating::Block:
            s.blockRating = !ql11 ? 211 : 236;
            break;
        case Rating::Defence:
            s.defenceRating = !ql11 ? 211 : 236;
            break;
        case Rating::Evade:
            s.evadeRating = !ql11 ? 196 : 213;
            break;
        case Rating::PhysProt:
            s.physProtRating = !ql11 ? 195 : 210;
            break;
        case Rating::MagProt:
            s.magProtRating = !ql11 ? 195 : 210;
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

Stats Gear::splitStatOf(Gear::Slot slot, Rating r, bool ql11)
{
    return singleStatOf(slot, r, ql11) * 0.5f;
}

void Gear::setNeckQL11()
{
    auto &p = pieces[MajorMid];
    p.status = SlotStatus::Free;
    p.set(PrimaryStat::Attack, TalismanQuality::QL11);
    p.signet = Signets::Major::Violence();
}

void Gear::setNeckWoodcutters()
{
    auto &p = pieces[MajorMid];
    p.fix(Rating::CritPower, false);
    p.set(PrimaryStat::Attack, TalismanQuality::QL10_9);
    p.signet = Signets::Major::WoodcuttersWrath();
}

void Gear::setNeckEgon()
{
    auto &p = pieces[MajorMid];
    p.status = SlotStatus::Free;
    p.set(PrimaryStat::Attack, TalismanQuality::QL10_9);
    p.signet = Signets::Major::EgonPendant();
}

void Gear::setFingerQL11()
{
    auto &p = pieces[MajorLeft];
    p.status = SlotStatus::Free;
    p.set(PrimaryStat::Attack, TalismanQuality::QL11);
    p.signet = Signets::Major::Violence();
}

void Gear::setFingerConey()
{
    auto &p = pieces[MajorLeft];
    p.status = SlotStatus::Free;
    p.set(PrimaryStat::Heal, TalismanQuality::QL10_9);
    p.signet = Signets::Major::ConeyIslandBand();
}

void Gear::Piece::fix(Rating r, bool ql11)
{
    status = SlotStatus::Fixed;
    auto s = stats.getPrimaryPart();
    stats = singleStatOf(slot, r, ql11) + s;
}

void Gear::Piece::fix(Rating r1, Rating r2, bool ql11)
{
    status = SlotStatus::Fixed;
    auto s = stats.getPrimaryPart();
    stats = splitStatOf(slot, r1, ql11) + splitStatOf(slot, r2, ql11) + s;
}

void Gear::Piece::free(Rating r, bool ql11)
{
    status = SlotStatus::Free;
    auto s = stats.getPrimaryPart();
    stats = singleStatOf(slot, r, ql11) + s;
}

void Gear::Piece::free(Rating r1, Rating r2, bool ql11)
{
    status = SlotStatus::Free;
    auto s = stats.getPrimaryPart();
    stats = splitStatOf(slot, r1, ql11) + splitStatOf(slot, r2, ql11) + s;
}

void Gear::Piece::set(PrimaryStat stat, TalismanQuality q)
{
    stats.attackRating = 0;
    stats.healRating = 0;
    stats.health = 0;

    int val = 0;

    switch (q)
    {
    case TalismanQuality::QL10_4:
        switch (slot)
        {
        case Slot::Head:
            switch (stat)
            {
            case PrimaryStat::Attack:
            case PrimaryStat::Heal:
                val = 735;
                break;
            case PrimaryStat::HP:
                val = 2476;
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
                val = 664;
                break;
            case PrimaryStat::HP:
                val = 2237;
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
                val = 427;
                break;
            case PrimaryStat::HP:
                val = 1438;
                break;
            default:
                assert(0);
            }
            break;
        case Slot::Weapon:
            val = 446;
            break;
        default:
            assert(0);
        }
        break;

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

jsonxx::Object Gear::Piece::toJson() const
{
    jsonxx::Object o;
    o << "Free" << (status == SlotStatus::Free);
    o << "Signet" << signet.name();
    o << "AR" << stats.attackRating;
    o << "Crit" << stats.critRating;
    o << "Crit Power" << stats.critPowerRating;
    o << "Pen" << stats.penRating;
    o << "Hit" << stats.hitRating;
    return o;
}

void Gear::Piece::fromJson(const jsonxx::Object &o)
{
    auto allSignets = Signets::all();

    using namespace jsonxx;
    status = o.get<Boolean>("Free", false) ? SlotStatus::Free : SlotStatus::Fixed;
    stats.attackRating = (int)o.get<Number>("AR", 0);
    stats.critRating = (int)o.get<Number>("Crit", 0);
    stats.critPowerRating = (int)o.get<Number>("Crit Power", 0);
    stats.penRating = (int)o.get<Number>("Pen", 0);
    stats.hitRating = (int)o.get<Number>("Hit", 0);
    auto sig = o.get<String>("Signet", "");
    if (sig.empty())
        signet = Signets::empty();
    else
    {
        bool found = false;
        for (auto const &s : allSignets)
            if (s.name() == sig)
            {
                found = true;
                signet = s;
                break;
            }
        if (!found)
            std::cerr << "Unknown signet " << sig << std::endl;
    }
}
