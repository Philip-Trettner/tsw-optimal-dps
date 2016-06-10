#include "Skills.hh"

#include "SkillTable.hh"

float Skills::scaling(const string &name)
{
    return SkillTable::scaling(name);
}

std::vector<Skill> Skills::all()
{
    return {
        Pistol::TheBusiness(),    //
        Pistol::HairTrigger(),    //
        Pistol::Shootout(),       //
        Pistol::Collaboration(),  //
        Pistol::Big45(),          //
        Pistol::DirtyTricks(),    //
        Pistol::BondStrongBond(), //
        Pistol::GunCrazy(),       //
        Pistol::StartAndFinish(), //
        Pistol::Marked(),         //
        Pistol::GunFu(),          //
        Pistol::BulletBallet(),   //

        Shotgun::Striker(),         //
        Shotgun::SingleBarrel(),    //
        Shotgun::SureShot(),        //
        Shotgun::RagingBullet(),    //
        Shotgun::PointBlank(),      //
        Shotgun::Kneecapper(),      //
        Shotgun::LockStockBarrel(), //
        Shotgun::Takedown(),        //
        Shotgun::Bombardment(),     //
        Shotgun::ShotgunWedding(),  //

        Rifle::Shellshocker(),    //
        Rifle::SafetyOff(),       //
        Rifle::TriggerHappy(),    //
        Rifle::ThreeRoundBurst(), //
        Rifle::RedMist(),         //
        Rifle::LockAndLoad(),     //
        Rifle::FireInTheHole(),   //

        Chaos::RunRampant(),         //
        Chaos::FourHorsemen(),       //
        Chaos::AmorFati(),           //
        Chaos::CallForEris(),        //
        Chaos::HandOfChange(),       //
        Chaos::DominoEffect(),       //
        Chaos::Paradox(),            //
        Chaos::PullingTheStrings(),  //
        Chaos::PrisonerOfFate(),     //
        Chaos::SufferingAndSolace(), //
        Chaos::ChaoticPull(),        //
        Chaos::EyeOfPandemonium(),   //

        Blade::DelicateStrike(), //
        Blade::DancingBlade(),   //
        Blade::FourSeasons(),    //
        Blade::BalancedBlade(),  //
        Blade::BindingWounds(),  //
        Blade::GrassCutter(),    //
        Blade::SlingBlade(),     //
        Blade::StunningSwirl(),  //
        Blade::SteelEcho(),      //

        Hammer::Smash(),        //
        Hammer::MoltenSteel(),  //
        Hammer::FullMomentum(), //
        Hammer::Haymaker(),     //
        Hammer::Shockwave(),    //
        Hammer::BoneBreaker(),  //
        Hammer::Eruption(),     //
        Hammer::StonesThrow(),  //
        Hammer::GrandSlam(),    //

        Fist::SeeRed(),         //
        Fist::Reckless(),       //
        Fist::Claw(),           //
        Fist::WildAtHeart(),    //
        Fist::PreyOnTheWeak(),  //
        Fist::OneTwo(),         //
        Fist::TearEmUp(),       //
        Fist::GoForTheThroat(), //
        // TODO: Gore + afflict? - STILL SUX

        Elemental::Whiteout(),               //
        Elemental::Blaze(),                  //
        Elemental::Combust(),                //
        Elemental::ElectricalBolt(),         //
        Elemental::FlameStrike(),            //
        Elemental::HardReset(),              //
        Elemental::Ignition(),               //
        Elemental::MoltenEarth(),            //
        Elemental::Overload(),               //
        Elemental::Shock(),                  //
        Elemental::ThorsHammer(),            //
        Elemental::PowerLine(),              //
        Elemental::FireManifestation(),      //
        Elemental::LightningManifestation(), //
        Elemental::AnimaCharge(),            //

        Blood::Bloodline(),          //
        Blood::Bloodshot(),          //
        Blood::Exsanguinate(),       //
        Blood::BoilingBlood(),       //
        Blood::CardiacArrest(),      //
        Blood::Cannibalize(),        //
        Blood::Plague(),             //
        Blood::Contaminate(),        //
        Blood::LeftHandOfDarkness(), //
                                     // TODO: Blood Spike

        Chainsaw::Timber(), //
    };
}
