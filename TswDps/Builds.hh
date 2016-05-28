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
    static Build hammerTest()
    {
        Build b;
        b.rotation = DefaultRotation::create();

        b.skills = {{
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

        // gear
        b.gear.loadStandardDpsGear();

        b.gear.leftWeapon = Weapon::Hammer;
        b.gear.pieces[Gear::WeaponLeft].signet = Signets::HeadWeapon::Abuse();

        b.gear.rightWeapon = Weapon::Shotgun;
        b.gear.pieces[Gear::WeaponRight].signet = Signets::HeadWeapon::Aggression();

        return b;
    }

	static Build currTest()
	{
		Build b;
		b.rotation = DefaultRotation::create();

		b.skills = { {
				// skills
				Skills::Pistol::HairTrigger(),      //
				Skills::Shotgun::Bombardment(),        //
			},
			{
				// augs
				Augments::Brutal(),   //
				Augments::Fierce(),   //
				Augments::Grievous(), //
				Augments::Piercing(), //
				Augments::Accurate(), //
			},
			{
				// passives
			} };

		// gear
		b.gear.loadStandardDpsGear();

		b.gear.leftWeapon = Weapon::Pistol;
		b.gear.pieces[Gear::WeaponLeft].signet = Signets::HeadWeapon::Aggression();

		b.gear.rightWeapon = Weapon::Shotgun;
		b.gear.pieces[Gear::WeaponRight].signet = Signets::HeadWeapon::Abuse();
		return b;
	}

	static Build currMaxPistolShotgun()
	{
		Build b;
		b.rotation = DefaultRotation::create();

		b.skills = { {
				// skills
				Skills::Pistol::HairTrigger(),      //
				Skills::Pistol::StartAndFinish(),   //
				Skills::Shotgun::RagingBullet(),    //
				Skills::Shotgun::Kneecapper(),      //
				Skills::Shotgun::SureShot(),        //
				Skills::Shotgun::LockStockBarrel(), //
			},
			{
				// augs
				Augments::Brutal(),   //
				Augments::Fierce(),   //
				Augments::Grievous(), //
				Augments::Piercing(), //
				Augments::Accurate(), //
			},
			{
				// passives
				Passives::Chaos::Gnosis(),             //
				Passives::Blood::IronMaiden(),         //
				Passives::Elemental::ElementalForce(), //
				Passives::Pistol::OneInTheChamber(),   //
				Passives::Blade::SuddenReturn(),       //
				Passives::Blade::FortunateStrike(),    //
				Passives::Elemental::LiveWire(),       //
			} };

		// gear
		b.gear.loadStandardDpsGear();

		b.gear.leftWeapon = Weapon::Pistol;
		b.gear.pieces[Gear::WeaponLeft].signet = Signets::HeadWeapon::Aggression();

		b.gear.rightWeapon = Weapon::Shotgun;
		b.gear.pieces[Gear::WeaponRight].signet = Signets::HeadWeapon::Abuse();
		return b;
	}

    static Build procHairtriggerOnly()
    {
        Build b;
        b.rotation = DefaultRotation::create();

        b.skills = {{
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
                        // Passives::Elemental::LiveWire(),       //
                        Passives::Hammer::Thunderstruck(), //
                    }};

        // gear
        b.gear.loadStandardDpsGear();

        b.gear.leftWeapon = Weapon::Pistol;
        b.gear.pieces[Gear::WeaponLeft].signet = Signets::HeadWeapon::Aggression();

        b.gear.rightWeapon = Weapon::Shotgun;
        b.gear.pieces[Gear::WeaponRight].signet = Signets::HeadWeapon::Abuse();
        return b;
    }

    static Build procBurstChaosRifle()
    {
        Build b;
        b.rotation = DefaultRotation::create();

        b.skills = {{
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

        // gear
        b.gear.loadStandardDpsGear();

        b.gear.leftWeapon = Weapon::Chaos;
        b.gear.pieces[Gear::WeaponLeft].signet = Signets::HeadWeapon::Aggression();

        b.gear.rightWeapon = Weapon::Rifle;
        b.gear.pieces[Gear::WeaponRight].signet = Signets::HeadWeapon::Aggression();
        return b;
    }

    static Build procBurstChaosFist()
    {
        Build b;
        b.rotation = DefaultRotation::create();

        b.skills = {{
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

        // gear
        b.gear.loadStandardDpsGear();

        b.gear.leftWeapon = Weapon::Chaos;
        b.gear.pieces[Gear::WeaponLeft].signet = Signets::HeadWeapon::Aggression();

        b.gear.rightWeapon = Weapon::Fist;
        b.gear.pieces[Gear::WeaponRight].signet = Signets::HeadWeapon::Aggression();
        return b;
    }

private:
    Builds() = delete;
};
