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

        Chaos::RunRampant(),   //
        Chaos::FourHorsemen(), //
		Chaos::AmorFati(),     //
		Chaos::CallForEris(),     //
		Chaos::HandOfChange(),     //
		Chaos::DominoEffect(),     //
		Chaos::Paradox(),     //
		Chaos::PullingTheStrings(),     //
		Chaos::PrisonerOfFate(),     //
		Chaos::SufferingAndSolace(),     //

        Hammer::Smash(),        //
        Hammer::MoltenSteel(),  //
        Hammer::FullMomentum(), //

        Fist::SeeRed(),   //
        Fist::Reckless(), //

        Chainsaw::Timber(), //
    };
}
