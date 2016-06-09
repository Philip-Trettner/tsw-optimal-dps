#include <gtest/gtest.h>

#include "CombatLog.hh"
#include "Passives.hh"
#include "Augments.hh"
#include "Simulation.hh"
#include "Skills.hh"

TEST(Manifestation, CorrectWeaponStats)
{
    Simulation s;
    s.skills = { { Skills::Pistol::HairTrigger(), Skills::Elemental::LightningManifestation() },{},{} };
    s.gear.setWeapons(Weapon::Pistol, Weapon::Elemental);

    {
        StatLog log;
        s.log = &log;

        s.init();
        s.simulate(timestr("1h"));

        auto mani = log[Skills::Elemental::LightningManifestation()];
        auto trigger = log[Skills::Pistol::HairTrigger()];
        ASSERT_GT(mani.blocks, 0);
        ASSERT_GT(trigger.blocks, 0);
    }

    s.gear.pieces[Gear::WeaponLeft].stats.penRating = 0;
    s.gear.pieces[Gear::WeaponRight].stats.penRating = 1000;

    {
        StatLog log;
        s.log = &log;

        s.init();
        s.simulate(timestr("1h"));

        auto mani = log[Skills::Elemental::LightningManifestation()];
        auto trigger = log[Skills::Pistol::HairTrigger()];
        ASSERT_GT(mani.blocks, 0);
        ASSERT_GT(trigger.blocks, 0);
    }

    s.gear.pieces[Gear::WeaponLeft].stats.penRating = 1000;
    s.gear.pieces[Gear::WeaponRight].stats.penRating = 0;

    {
        StatLog log;
        s.log = &log;

        s.init();
        s.simulate(timestr("1h"));

        auto mani = log[Skills::Elemental::LightningManifestation()];
        auto trigger = log[Skills::Pistol::HairTrigger()];
        ASSERT_EQ(mani.blocks, 0);
        ASSERT_EQ(trigger.blocks, 0);
    }
}

TEST(Manifestation, CorrectAugmentStats)
{
    Simulation s;
    s.skills = { { Skills::Pistol::HairTrigger(), Skills::Elemental::LightningManifestation() },{ Augments::Piercing(), Augments::Piercing() },{} };
    s.gear.setWeapons(Weapon::Pistol, Weapon::Elemental);

    s.skills.augments[0].bonusStats.addedPenChance = 0.0f;
    s.skills.augments[1].bonusStats.addedPenChance = 0.0f;

    {
        StatLog log;
        s.log = &log;

        s.init();
        s.simulate(timestr("1h"));

        auto mani = log[Skills::Elemental::LightningManifestation()];
        auto trigger = log[Skills::Pistol::HairTrigger()];
        ASSERT_GT(mani.blocks, 0);
        ASSERT_GT(trigger.blocks, 0);
    }

    s.skills.augments[0].bonusStats.addedPenChance = 40.0f;
    s.skills.augments[1].bonusStats.addedPenChance = 0.0f;

    {
        StatLog log;
        s.log = &log;

        s.init();
        s.simulate(timestr("1h"));

        auto mani = log[Skills::Elemental::LightningManifestation()];
        auto trigger = log[Skills::Pistol::HairTrigger()];
        ASSERT_GT(mani.blocks, 0);
        ASSERT_EQ(trigger.blocks, 0);
    }

    s.skills.augments[0].bonusStats.addedPenChance = 0.0f;
    s.skills.augments[1].bonusStats.addedPenChance = 40.0f;

    {
        StatLog log;
        s.log = &log;

        s.init();
        s.simulate(timestr("1h"));

        auto mani = log[Skills::Elemental::LightningManifestation()];
        auto trigger = log[Skills::Pistol::HairTrigger()];
        ASSERT_EQ(mani.blocks, 0);
        ASSERT_GT(trigger.blocks, 0);
    }
}

TEST(Bombardment, CorrectWeaponStats)
{
    Simulation s;
    s.skills = { { Skills::Pistol::HairTrigger(), Skills::Shotgun::Bombardment() },{},{} };
    s.gear.setWeapons(Weapon::Pistol, Weapon::Shotgun);

    {
        StatLog log;
        s.log = &log;

        s.init();
        s.simulate(timestr("1h"));

        auto mani = log[Skills::Shotgun::Bombardment()];
        auto trigger = log[Skills::Pistol::HairTrigger()];
        ASSERT_GT(mani.blocks, 0);
        ASSERT_GT(trigger.blocks, 0);
    }

    s.gear.pieces[Gear::WeaponLeft].stats.penRating = 0;
    s.gear.pieces[Gear::WeaponRight].stats.penRating = 1000;

    {
        StatLog log;
        s.log = &log;

        s.init();
        s.simulate(timestr("1h"));

        auto mani = log[Skills::Shotgun::Bombardment()];
        auto trigger = log[Skills::Pistol::HairTrigger()];
        ASSERT_GT(mani.blocks, 0);
        ASSERT_GT(trigger.blocks, 0);
    }

    s.gear.pieces[Gear::WeaponLeft].stats.penRating = 1000;
    s.gear.pieces[Gear::WeaponRight].stats.penRating = 0;

    {
        StatLog log;
        s.log = &log;

        s.init();
        s.simulate(timestr("1h"));

        auto mani = log[Skills::Shotgun::Bombardment()];
        auto trigger = log[Skills::Pistol::HairTrigger()];
        ASSERT_EQ(mani.blocks, 0);
        ASSERT_EQ(trigger.blocks, 0);
    }
}

TEST(Bombardment, CorrectAugmentStats)
{
    Simulation s;
    s.skills = { { Skills::Pistol::HairTrigger(), Skills::Shotgun::Bombardment() },{ Augments::Piercing(), Augments::Piercing() },{} };
    s.gear.setWeapons(Weapon::Pistol, Weapon::Shotgun);

    s.skills.augments[0].bonusStats.addedPenChance = 0.0f;
    s.skills.augments[1].bonusStats.addedPenChance = 0.0f;

    {
        StatLog log;
        s.log = &log;

        s.init();
        s.simulate(timestr("1h"));

        auto mani = log[Skills::Shotgun::Bombardment()];
        auto trigger = log[Skills::Pistol::HairTrigger()];
        ASSERT_GT(mani.blocks, 0);
        ASSERT_GT(trigger.blocks, 0);
    }

    s.skills.augments[0].bonusStats.addedPenChance = 40.0f;
    s.skills.augments[1].bonusStats.addedPenChance = 0.0f;

    {
        StatLog log;
        s.log = &log;

        s.init();
        s.simulate(timestr("1h"));

        auto mani = log[Skills::Shotgun::Bombardment()];
        auto trigger = log[Skills::Pistol::HairTrigger()];
        ASSERT_GT(mani.blocks, 0);
        ASSERT_EQ(trigger.blocks, 0);
    }

    s.skills.augments[0].bonusStats.addedPenChance = 0.0f;
    s.skills.augments[1].bonusStats.addedPenChance = 40.0f;

    {
        StatLog log;
        s.log = &log;

        s.init();
        s.simulate(timestr("1h"));

        auto mani = log[Skills::Shotgun::Bombardment()];
        auto trigger = log[Skills::Pistol::HairTrigger()];
        ASSERT_EQ(mani.blocks, 0);
        ASSERT_GT(trigger.blocks, 0);
    }
}
