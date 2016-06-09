#include <gtest/gtest.h>

#include "Simulation.hh"
#include "Skills.hh"
#include "Passives.hh"
#include "Signets.hh"
#include "CombatLog.hh"

TEST(Tokens, ActiveTest)
{
    Simulation s;
    s.skills = {{Skills::Pistol::HairTrigger()},
                {},
                {}};
    s.gear.setWeapons(Weapon::Pistol, Weapon::Chaos);
    s.gear.pieces[Gear::MinorLeft].signet = Signets::Minor::SubwayTokens();

    s.simulate(timestr("1h"));
}
