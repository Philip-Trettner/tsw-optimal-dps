#include <gtest/gtest.h>

#include "Simulation.hh"
#include "Skills.hh"
#include "Passives.hh"
#include "CombatLog.hh"

TEST(Procs, OncePerSec)
{
    Simulation s;
    s.skills = {{Skills::Pistol::HairTrigger()},
                {},
                {Passives::Pistol::OneInTheChamber(), Passives::Hammer::Thunderstruck(), Passives::Blade::SuddenReturn()}};
    s.gear.setWeapons(Weapon::Pistol, Weapon::Chaos);
    s.potionStats.addedCritChance = .8;
    s.potionStats.addedPenChance = .8;

    StatLog log;
    s.log = &log;

    s.simulate(timestr("1h"));

    ASSERT_LE(log[EffectSlot::OneInTheChamber].hits, s.totalTimeAccum / 60);
    ASSERT_LE(log[EffectSlot::Thunderstruck].hits, s.totalTimeAccum / 60);
    ASSERT_LE(log[EffectSlot::SuddenReturn].hits, s.totalTimeAccum / 60);

    ASSERT_GE(log[EffectSlot::OneInTheChamber].hits, s.totalTimeAccum / 60 * .4);
    ASSERT_GE(log[EffectSlot::Thunderstruck].hits, s.totalTimeAccum / 60 * .4);
    ASSERT_GE(log[EffectSlot::SuddenReturn].hits, s.totalTimeAccum / 60 * .4);
}

TEST(Procs, AdditiveDmg)
{
    Simulation s;
    s.skills = {{Skills::Shotgun::Striker(), Skills::Elemental::LightningManifestation(), Skills::Shotgun::Bombardment()},
                {},
                {Passives::Pistol::OneInTheChamber(), Passives::Hammer::Thunderstruck(), Passives::Blade::SuddenReturn()}};
    s.gear.setWeapons(Weapon::Shotgun, Weapon::Elemental);
    s.potionStats.addedCritChance = .5;
    s.potionStats.addedPenChance = .5;
    s.lowVarianceMode = true;

    auto striker = Skills::Shotgun::Striker();
    auto mani = Skills::Elemental::LightningManifestation();
    auto bomb = Skills::Shotgun::Bombardment();

    StatLog blog;
    s.log = &blog;

    s.init();
    s.simulate(timestr("1h"));

    auto epsdmg = blog[EffectSlot::OneInTheChamber].totalDmg * .2;

    s.potionStats.additiveDamage = 1; // double dmg

    {
        StatLog log;
        s.log = &log;

        s.init();
        s.simulate(timestr("1h"));

        ASSERT_NEAR(blog[EffectSlot::OneInTheChamber].totalDmg, log[EffectSlot::OneInTheChamber].totalDmg, epsdmg);
        ASSERT_NEAR(blog[EffectSlot::Thunderstruck].totalDmg, log[EffectSlot::Thunderstruck].totalDmg, epsdmg);
        ASSERT_NEAR(blog[EffectSlot::SuddenReturn].totalDmg, log[EffectSlot::SuddenReturn].totalDmg, epsdmg);

        ASSERT_NEAR(2 * blog[striker].totalDmg, log[striker].totalDmg, epsdmg);
        ASSERT_NEAR(2 * blog[mani].totalDmg, log[mani].totalDmg, epsdmg);
        ASSERT_NEAR(2 * blog[bomb].totalDmg, log[bomb].totalDmg, epsdmg);
    }
}

TEST(Procs, MultiplicativeDmg)
{
    Simulation s;
    s.skills = {{Skills::Shotgun::Striker(), Skills::Elemental::LightningManifestation(), Skills::Shotgun::Bombardment()},
                {},
                {Passives::Pistol::OneInTheChamber(), Passives::Hammer::Thunderstruck(), Passives::Blade::SuddenReturn()}};
    s.gear.setWeapons(Weapon::Shotgun, Weapon::Elemental);
    s.potionStats.addedCritChance = .5;
    s.potionStats.addedPenChance = .5;
    s.lowVarianceMode = true;

    auto striker = Skills::Shotgun::Striker();
    auto mani = Skills::Elemental::LightningManifestation();
    auto bomb = Skills::Shotgun::Bombardment();

    StatLog blog;
    s.log = &blog;

    s.init();
    s.simulate(timestr("1h"));

    auto epsdmg = blog[EffectSlot::OneInTheChamber].totalDmg * .2;

    s.potionStats.multiplicativeDamage = 1; // double dmg

    {
        StatLog log;
        s.log = &log;

        s.init();
        s.simulate(timestr("1h"));

        ASSERT_NEAR(2 * blog[EffectSlot::OneInTheChamber].totalDmg, log[EffectSlot::OneInTheChamber].totalDmg, epsdmg);
        ASSERT_NEAR(2 * blog[EffectSlot::Thunderstruck].totalDmg, log[EffectSlot::Thunderstruck].totalDmg, epsdmg);
        ASSERT_NEAR(2 * blog[EffectSlot::SuddenReturn].totalDmg, log[EffectSlot::SuddenReturn].totalDmg, epsdmg);

        ASSERT_NEAR(2 * blog[striker].totalDmg, log[striker].totalDmg, epsdmg);
        ASSERT_NEAR(2 * blog[mani].totalDmg, log[mani].totalDmg, epsdmg);
        ASSERT_NEAR(2 * blog[bomb].totalDmg, log[bomb].totalDmg, epsdmg);
    }
}

TEST(SteelEcho, OncePerSec)
{
    Simulation s;
    s.skills = {{Skills::Blade::GrassCutter(), Skills::Blade::SteelEcho(), Skills::Blade::DancingBlade()},
                {},
                {}};
    s.gear.setWeapons(Weapon::Pistol, Weapon::Blade);

    StatLog log;
    s.log = &log;

    s.simulate(7 * 60);

    ASSERT_EQ(log[EffectSlot::SteelEcho].hits, 3);
}
