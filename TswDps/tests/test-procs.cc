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
}
