#include "Build.hh"

#include "Skills.hh"
#include "Passives.hh"
#include "Augments.hh"


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
            std::cout << ", only build shorty before DABS";
        std::cout << std::endl;
    }

    // TODO: potion?
}
