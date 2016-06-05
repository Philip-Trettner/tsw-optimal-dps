#include "Build.hh"

#include "Skills.hh"
#include "Passives.hh"
#include "Augments.hh"

#include <fstream>
#include <iostream>

namespace
{
string shortStatDump(Stats const& s, bool withBase = true)
{
    string ss;
    if (s.attackRating > 0 && withBase)
        ss += (ss.empty() ? "" : ", ") + std::to_string(s.attackRating) + " AR";
    if (s.penRating > 0)
        ss += (ss.empty() ? "" : ", ") + std::to_string(s.penRating) + " Pen";
    if (s.critRating > 0)
        ss += (ss.empty() ? "" : ", ") + std::to_string(s.critRating) + " CR";
    if (s.critPowerRating > 0)
        ss += (ss.empty() ? "" : ", ") + std::to_string(s.critPowerRating) + " CP";
    return ss;
}
string shortStatDump(Gear::Piece const& p)
{
    return shortStatDump(p.stats, false);
}
}

void Build::shortDump() const
{
    std::cout << "Actives:  ";
    auto first = true;
    for (auto i = 0; i < SKILL_CNT; ++i)
    {
        auto const& s = skills.skills[i];
        if (!s.name.empty())
        {
            if (first)
                first = false;
            else
                std::cout << ", ";
            std::cout << s.name;
            if (!skills.augments[i].name.empty())
                std::cout << " [" << skills.augments[i].name << "]";
        }
    }
    std::cout << std::endl;

    std::cout << "Passives: ";
    first = true;
    for (auto const& s : skills.passives)
        if (!s.name.empty())
        {
            if (first)
                first = false;
            else
                std::cout << ", ";
            std::cout << s.name;
        }
    std::cout << std::endl;

    std::cout << "Gear:     ";
    auto s = gear.gearStats();
    std::cout << s.attackRating << " AR, ";
	std::cout << s.hitRating << " Hit, ";
	std::cout << s.penRating << " Pen, ";
    std::cout << s.critRating << " CR, ";
    std::cout << s.critPowerRating << " CP, Neck: ";
    if (gear.pieces[Gear::MajorMid].signet.passive.effect == EffectSlot::MothersWrathStacks)
        std::cout << "Woodcutters";
    else if (gear.pieces[Gear::MajorMid].signet.passive.effect == EffectSlot::EgonPendant)
        std::cout << "Amulet of Yuggoth";
    else
        std::cout << "QL11 + Violence";
    std::cout << std::endl;

    std::cout << "Weapons:  ";
    std::cout << shortStatDump(gear.pieces[Gear::WeaponLeft]) << " for " << to_string(gear.leftWeapon) << ", ";
    std::cout << shortStatDump(gear.pieces[Gear::WeaponRight]) << " for " << to_string(gear.rightWeapon);
    std::cout << std::endl;

    std::cout << "Signets:  ";
    std::cout << gear.pieces[Gear::Head].signet.name() << " on Head, ";
    std::cout << gear.pieces[Gear::WeaponLeft].signet.name() << " on " << to_string(gear.leftWeapon) << ", ";
    std::cout << gear.pieces[Gear::WeaponRight].signet.name() << " on " << to_string(gear.rightWeapon);
    std::cout << std::endl;

    auto defRot = std::dynamic_pointer_cast<DefaultRotation>(rotation);
    if (defRot)
    {
        std::cout << "Rotation: ";
        std::cout << "use resources on " << defRot->minResourcesForLeftConsumer << " for " << to_string(gear.leftWeapon)
                  << " and " << defRot->minResourcesForRightConsumer << " for " << to_string(gear.rightWeapon);
        if (defRot->tryToConsumeOnBuffed)
            std::cout << " (But prefer consume on buffs)";
        std::cout << ", buffs are: [ ";
        if (defRot->considerBuffEF)
            std::cout << "EF ";
        if (defRot->considerBuffFF)
            std::cout << "FF ";
        if (defRot->considerBuffWC)
            std::cout << "WC ";
        std::cout << "]";
        if (defRot->consumeIfNotBloodOffering && (gear.leftWeapon == Weapon::Blood || gear.rightWeapon == Weapon::Blood))
            std::cout << ", force Blood Offering uptime";
        if (defRot->buildOnlyBeforeMajorBuffs)
            std::cout << ", only build shortly before DABS";
        std::cout << std::endl;
    }

    // TODO: potion?
}

jsonxx::Object Build::toJson() const
{
    jsonxx::Array ss;
    jsonxx::Array aa;
    for (auto i = 0; i < SKILL_CNT; ++i)
    {
        ss << skills.skills[i].name;
        aa << skills.augments[i].name;
    }
    jsonxx::Array pp;
    for (auto const& p : skills.passives)
        pp << p.name;
    jsonxx::Array gg;
    for (auto const& p : gear.pieces)
        gg << p.toJson();
    jsonxx::Object b;
    b << "Skills" << ss;
    b << "Augments" << aa;
    b << "Passives" << pp;
    b << "Left Weapon" << (int)gear.leftWeapon;
    b << "Right Weapon" << (int)gear.rightWeapon;
    b << "Gear" << gg;
    auto rot = std::dynamic_pointer_cast<DefaultRotation>(rotation);
    assert(rot && "not supported");
    b << "Rotation" << rot->toJson();
    return b;
}

void Build::fromJson(const jsonxx::Object& o)
{
    using namespace jsonxx;
    auto ss = o.get<Array>("Skills");
    auto aa = o.get<Array>("Augments");
    auto pp = o.get<Array>("Passives");
    auto gg = o.get<Array>("Gear");

    gear.leftWeapon = (Weapon)(int)o.get<Number>("Left Weapon", (int)gear.leftWeapon);
    gear.rightWeapon = (Weapon)(int)o.get<Number>("Right Weapon", (int)gear.rightWeapon);

    auto allSkills = Skills::all();
    auto allAugs = Augments::allDpsAugs();
    auto allPassives = Passives::all();

    skills.passives.resize(pp.size());

    for (auto i = 0u; i < ss.size(); ++i)
    {
        auto const& s = ss.get<String>(i);
        if (s.empty())
            skills.skills[i] = Skills::empty();
        else
        {
            bool found = false;
            for (auto const& sr : allSkills)
                if (sr.name == s)
                {
                    skills.skills[i] = sr;
                    found = true;
                    break;
                }
            if (!found)
                std::cerr << "Unknown skill " << s << std::endl;
        }
    }

    for (auto i = 0u; i < aa.size(); ++i)
    {
        auto const& s = aa.get<String>(i);
        if (s.empty())
            skills.augments[i] = Augments::empty();
        else
        {
            bool found = false;
            for (auto const& ar : allAugs)
                if (ar.name == s)
                {
                    skills.augments[i] = ar;
                    found = true;
                    break;
                }
            if (!found)
                std::cerr << "Unknown augment " << s << std::endl;
        }
    }

    for (auto i = 0u; i < pp.size(); ++i)
    {
        auto const& s = pp.get<String>(i);
        if (s.empty())
            skills.passives[i] = Passives::empty();
        else
        {
            bool found = false;
            for (auto const& pr : allPassives)
                if (pr.name == s)
                {
                    skills.passives[i] = pr;
                    found = true;
                    break;
                }
            if (!found)
                std::cerr << "Unknown passive " << s << std::endl;
        }
    }

    for (auto i = 0u; i < gg.size(); ++i)
    {
        auto const& go = gg.get<Object>(i);
        gear.pieces[i].fromJson(go);
    }

    auto defRot = std::dynamic_pointer_cast<DefaultRotation>(rotation);
    if (!defRot) defRot = DefaultRotation::create();
    defRot->fromJson(o.get<Object>("Rotation"));
    rotation = defRot;
}

void Build::fromFile(const string& filename)
{
    std::ifstream file(filename);
    if (!file.good())
    {
        std::cerr << "Unable to open " << filename;
        return;
    }
    jsonxx::Object o;
    o.parse(file);
    fromJson(o);
}
