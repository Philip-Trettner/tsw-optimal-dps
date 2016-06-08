#pragma once

#include "Augments.hh"
#include "Passives.hh"
#include "Rotation.hh"
#include "Signets.hh"
#include "Simulation.hh"
#include "Skills.hh"
#include "Skillset.hh"

struct Builds
{
    static Build fromFile(std::string const& filename)
    {
        Build b;
        b.gear.loadStandardDpsGear(); // for weapons
        b.fromFile(filename);
        return b;
    }

    static Build currTest()
    {
        Build b;
        auto rot = DefaultRotation::create();
        rot->buildOnlyBeforeMajorBuffs = false;
        b.rotation = rot;

        b.skills = {{
                        // skills
                        Skills::Chaos::RunRampant(),     //
                    },
                    {
                        // augs
                    },
                    {
                        // passives
                        // Passives::Shotgun::InvasiveMeasures(), //
                        Passives::Fist::Ferocity(),     //
                    }};

        // gear
        b.gear.loadStandardDpsGear();

        // hammer ele, i swear, over 9000
        b.gear.leftWeapon = Weapon::Blood;
        b.gear.rightWeapon = Weapon::Chaos;
        return b;
    }

private:
    Builds() = delete;
};
