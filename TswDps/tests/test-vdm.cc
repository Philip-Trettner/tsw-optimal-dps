#include <gtest/gtest.h>

#include "Simulation.hh"
#include "CombatLog.hh"
#include "Builds.hh"
#include "SkillTable.hh"

TEST(VDM, Sanity)
{
    for (auto const& s : Skills::all())
        ASSERT_NE(SkillTable::vdmID(s.name), "");

    for (auto const& s : Passives::all())
        ASSERT_NE(SkillTable::vdmID(s.name), "");

    for (auto const& s : Augments::all())
        ASSERT_NE(SkillTable::vdmID(s.name + " Augment Resonator"), "");
}
