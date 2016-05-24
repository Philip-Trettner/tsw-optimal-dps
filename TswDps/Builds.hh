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
    static void hammerTest(Simulation& sim)
    {
        sim.rotation = DefaultRotation::create();

        sim.skills = {{
                          // skills
                          Skills::Hammer::Smash(),            //
                          Skills::Shotgun::RagingBullet(),    //
                          Skills::Hammer::MoltenSteel(),      //
                          Skills::Shotgun::PointBlank(),      //
                          Skills::Hammer::FullMomentum(),     //
                          Skills::Shotgun::LockStockBarrel(), //
                          Skills::Chainsaw::Timber(),         //
                      },
                      {
                          // augs
                          Augments::Brutal(),    //
                          Augments::Focused(),   //
                          Augments::Ferocious(), //
                          Augments::Fierce(),    //
                          Augments::empty(),     //
                          Augments::empty(),     //
                          Augments::empty(),     //
                      },
                      {
                          // passives
                          // Passives::Hammer::Brawler(),           //
                          Passives::Blade::SuddenReturn(),       //
                          Passives::Blood::IronMaiden(),         //
                          Passives::Elemental::ElementalForce(), //
                          Passives::Blade::TwistTheKnife(),      //
                          Passives::Rifle::Lethality(),          //
                          Passives::Hammer::Thunderstruck(),     //
                          Passives::Pistol::OneInTheChamber(),   //
                      }};

        // weapons
        sim.gear.leftWeapon = Weapon::Hammer;
        sim.gear.pieces[Gear::WeaponLeft].signet = Signets::HeadWeapon::Abuse();

        sim.gear.rightWeapon = Weapon::Shotgun;
        sim.gear.pieces[Gear::WeaponRight].signet = Signets::HeadWeapon::Aggression();
    }

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
