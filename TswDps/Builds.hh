#pragma once

#include "Skills.hh"
#include "Augments.hh"
#include "Passives.hh"
#include "Skillset.hh"

struct Builds
{
    static Skillset procHairtriggerOnly()
    {
        return {{
                    // skills
                    Skills::Pistol::HairTrigger(),  //
                    Skills::Shotgun::OutForAKill(), //
                    Skills::Pistol::Shootout(),     //
                },
                {
                    // augs
                    Augments::Brutal(),    //
                    Augments::Piercing(),  //
                    Augments::Ferocious(), //
                },
                {
                    // passives
                    Passives::Hammer::Brawler(),           //
                    Passives::Blood::IronMaiden(),         //
                    Passives::Elemental::ElementalForce(), //
                    Passives::Pistol::OneInTheChamber(),   //
                    Passives::Blade::SuddenReturn(),       //
                    Passives::Blade::FortunateStrike(),    //
                    Passives::Hammer::Thunderstruck(),     //
                }};
    }

private:
    Builds() = delete;
};
