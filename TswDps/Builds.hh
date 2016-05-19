#pragma once

#include "Skills.hh"
#include "Augments.hh"
#include "Passives.hh"
#include "Skillset.hh"
#include "Simulation.hh"
#include "Rotation.hh"
#include "Signets.hh"

struct Builds
{
    static void procHairtriggerOnly(Simulation& sim)
    {
        sim.rotation = DefaultRotation::create();

        sim.skills = {{
                          // skills
                          Skills::Pistol::HairTrigger(), //
                      },
                      {
                          // augs
                          Augments::Brutal(), //
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

        // weapons
        sim.gear.leftWeapon = Weapon::Pistol;
        sim.gear.pieces[Gear::WeaponLeft].signet = Signets::HeadWeapon::Aggression();

        sim.gear.rightWeapon = Weapon::Shotgun;
        sim.gear.pieces[Gear::WeaponRight].signet = Signets::HeadWeapon::Abuse();
    }

    static void procBurstChaosRifle(Simulation& sim)
    {
        sim.rotation = DefaultRotation::create();

        sim.skills = {{
                          // skills
                          Skills::Chaos::RunRampant(),   //
                          Skills::Chaos::FourHorsemen(), //
                          Skills::Rifle::Shellshocker(), //
                      },
                      {
                          // augs
                          Augments::Brutal(),  //
                          Augments::Focused(), //
                          Augments::Fierce(),  //
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

        // weapons
        sim.gear.leftWeapon = Weapon::Chaos;
        sim.gear.pieces[Gear::WeaponLeft].signet = Signets::HeadWeapon::Aggression();

        sim.gear.rightWeapon = Weapon::Rifle;
        sim.gear.pieces[Gear::WeaponRight].signet = Signets::HeadWeapon::Aggression();
    }

    static void procBurstChaosFist(Simulation& sim)
    {
        sim.rotation = DefaultRotation::create();

        sim.skills = {{
                          // skills
                          Skills::Chaos::RunRampant(),   //
                          Skills::Chaos::FourHorsemen(), //
                          Skills::Fist::Reckless(),      //
                          Skills::Chaos::AmorFati(),     //
                          Skills::Fist::SeeRed(),        //
                          Skills::empty(),               //
                          Skills::empty(),               //
                          Skills::Chainsaw::Timber(),    //
                      },
                      {
                          // augs
                          Augments::Brutal(),   //
                          Augments::Focused(),  //
                          Augments::Accurate(), //
                          Augments::empty(),    //
                          Augments::Fierce(),   //
                          Augments::empty(),    //
                          Augments::empty(),    //
                      },
                      {
                          // passives
                          // Passives::Hammer::Brawler(),           //
                          Passives::Chaos::Gnosis(),             //
                          Passives::Blood::IronMaiden(),         //
                          Passives::Elemental::ElementalForce(), //
                          Passives::Pistol::OneInTheChamber(),   //
                          Passives::Blade::SuddenReturn(),       //
                          Passives::Blade::FortunateStrike(),    //
                          Passives::Hammer::Thunderstruck(),     //
                          Passives::empty(),                     //
                      }};

        // weapons
        sim.gear.leftWeapon = Weapon::Chaos;
        sim.gear.pieces[Gear::WeaponLeft].signet = Signets::HeadWeapon::Aggression();

        sim.gear.rightWeapon = Weapon::Fist;
        sim.gear.pieces[Gear::WeaponRight].signet = Signets::HeadWeapon::Aggression();
    }

private:
    Builds() = delete;
};
