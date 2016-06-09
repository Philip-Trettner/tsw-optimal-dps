#include <gtest/gtest.h>

#include "Simulation.hh"
#include "Skills.hh"
#include "Signets.hh"
#include "Augments.hh"
#include "Passives.hh"
#include "CombatLog.hh"

TEST(Sanity, FixedDps)
{
    Simulation s;
    s.skills = {{
                    Skills::Blood::Bloodline(),                  //
                    Skills::Blood::Cannibalize(),                //
                    Skills::Elemental::AnimaCharge(),            //
                    Skills::Elemental::LightningManifestation(), //
                    Skills::Elemental::FireManifestation(),      //
                    Skills::Blood::Bloodshot(),                  //
                    Skills::Blood::CardiacArrest(),              //
                },
                {
                    Augments::Brutal(),    //
                    Augments::empty(),     //
                    Augments::empty(),     //
                    Augments::Piercing(),  //
                    Augments::Ferocious(), //
                    Augments::Focused(),   //
                    Augments::Accurate(),  //
                },
                {
                    Passives::Pistol::OneInTheChamber(),   //
                    Passives::Elemental::LiveWire(),       //
                    Passives::Elemental::ElementalForce(), //
                    Passives::Blood::IronMaiden(),         //
                    Passives::Rifle::Lethality(),          //
                    Passives::Blade::FortunateStrike(),    //
                    Passives::Blade::SuddenReturn(),       //
                }};
    s.gear.loadEmptyDpsGear();
    s.gear.setNeckWoodcutters();
    s.gear.setWeapons(Weapon::Blood, Weapon::Elemental);

    s.gear.pieces[Gear::Head].free(Rating::Hit);
    s.gear.pieces[Gear::Head].signet = Signets::HeadWeapon::Laceration();

    s.gear.pieces[Gear::MajorLeft].free(Rating::Crit);
    s.gear.pieces[Gear::MajorRight].free(Rating::Pen);

    s.gear.pieces[Gear::MinorLeft].free(Rating::Hit);
    s.gear.pieces[Gear::MinorLeft].signet = Signets::Minor::SubwayTokens();
    s.gear.pieces[Gear::MinorMid].free(Rating::Crit);
    s.gear.pieces[Gear::MinorMid].signet = Signets::Minor::Issue1p5();
    s.gear.pieces[Gear::MinorRight].free(Rating::CritPower);
    s.gear.pieces[Gear::MinorRight].signet = Signets::Minor::Issue1p5();

    s.gear.pieces[Gear::WeaponLeft].free(Rating::Crit);
    s.gear.pieces[Gear::WeaponLeft].signet = Signets::HeadWeapon::Aggression();
    s.gear.pieces[Gear::WeaponRight].free(Rating::Crit);
    s.gear.pieces[Gear::WeaponRight].signet = Signets::HeadWeapon::Aggression();

    s.potionStats.critRating = 119;

    auto rot = DefaultRotation::create();
    rot->buildOnlyBeforeMajorBuffs = false;
    rot->considerBuffEF = true;
    rot->considerBuffWC = true;
    rot->consumeIfNotBloodOffering = true;
    rot->minResourcesForLeftConsumer = 1;
    rot->minResourcesForRightConsumer = 1;
    s.rotation = rot;

    s.enemyInfo.allVulnerabilities = true;
    s.lowVarianceMode = true;

    s.simulate(timestr("6h"));

    ASSERT_NEAR(s.totalDPS(), 8234.79, 50);
}
