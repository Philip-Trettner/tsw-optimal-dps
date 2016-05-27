#include "Skills.hh"

#include "SkillTable.hh"

float Skills::scaling(const string &name)
{
    return SkillTable::scaling(name);
}

std::vector<Skill> Skills::all()
{
    return {
        Pistol::TheBusiness(), //
        Pistol::HairTrigger(), //
        Pistol::Shootout(),    //

        Shotgun::Striker(),         //
        Shotgun::SingleBarrel(),    //
        Shotgun::SureShot(),        //
        Shotgun::RagingBullet(),    //
        Shotgun::PointBlank(),      //
        Shotgun::Kneecapper(),      //
        Shotgun::LockStockBarrel(), //

        Rifle::Shellshocker(), //

        Chaos::RunRampant(),   //
        Chaos::FourHorsemen(), //
        Chaos::AmorFati(),     //

        Hammer::Smash(),        //
        Hammer::MoltenSteel(),  //
        Hammer::FullMomentum(), //

        Fist::SeeRed(),   //
        Fist::Reckless(), //

        Chainsaw::Timber(), //
    };
}
