#include <gtest/gtest.h>

#include "Simulation.hh"
#include "Skills.hh"
#include "Builds.hh"
#include "CombatLog.hh"

TEST(NoSelf, Mercurial)
{
    Simulation s;
    s.skills = {{Skills::Pistol::HairTrigger(), Skills::Elemental::PowerLine()},
                {Augments::empty(), Augments::empty()},
                {}};
    s.gear.setWeapons(Weapon::Pistol, Weapon::Elemental);

    // no merc
    {
        StatLog log;
        s.log = &log;

        int skills = 400;
        s.simulate(20 * 60 * (skills - 1) + 15 * 60);

        ASSERT_EQ(log[EffectSlot::PowerLineDetonation].hits, skills);
    }
    // merc on powerline
    {
        StatLog log;

        s.skills.augments[0] = Augments::empty();
        s.skills.augments[1] = Augments::Mercurial();

        s.log = &log;

        int skills = 400;
        s.simulate(20 * 60 * (skills - 1) + 15 * 60);

        ASSERT_EQ(log[EffectSlot::PowerLineDetonation].hits, skills);
    }
    // merc on hair trigger
    {
        StatLog log;

        s.skills.augments[0] = Augments::Mercurial();
        s.skills.augments[1] = Augments::empty();

        s.log = &log;

        int skills = 400;
        s.simulate(20 * 60 * (skills - 1) + 15 * 60);

        ASSERT_GT(log[EffectSlot::PowerLineDetonation].hits, skills * 1.05);
        ASSERT_LE(log[EffectSlot::PowerLineDetonation].hits, skills * 1.10);
    }
}

TEST(NoSelf, Accelerating)
{
    Simulation s;
    s.skills = {{Skills::Pistol::HairTrigger(), Skills::Elemental::PowerLine()},
                {Augments::empty(), Augments::empty()},
                {}};
    s.gear.setWeapons(Weapon::Pistol, Weapon::Elemental);

    // no acc
    {
        StatLog log;
        s.log = &log;

        int skills = 400;
        s.simulate(20 * 60 * (skills - 1) + 15 * 60);

        ASSERT_EQ(log[EffectSlot::PowerLineDetonation].hits, skills);
    }
    // acc on powerline
    {
        StatLog log;

        s.skills.augments[0] = Augments::empty();
        s.skills.augments[1] = Augments::Accelerating();

        s.log = &log;

        int skills = 400;
        s.simulate(20 * 60 * (skills - 1) + 15 * 60);

        ASSERT_EQ(log[EffectSlot::PowerLineDetonation].hits, skills);
    }
    // acc on hair trigger
    {
        StatLog log;

        s.skills.augments[0] = Augments::Accelerating();
        s.skills.augments[1] = Augments::empty();

        s.log = &log;

        int skills = 400;
        s.simulate(20 * 60 * (skills - 1) + 15 * 60);

        ASSERT_GT(log[EffectSlot::PowerLineDetonation].hits, skills * 1.05);
        ASSERT_LE(log[EffectSlot::PowerLineDetonation].hits, skills * 1.10);
    }
}
