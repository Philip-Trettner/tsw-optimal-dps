#include "Passives.hh"


std::vector<Passive> Passives::all()
{
    return {
        // Pistol [done]
        Pistol::SealTheDeal(),     //
        Pistol::OneInTheChamber(), //
        Pistol::Closer(),          //
        Pistol::QuickFortyFive(),  //
        Pistol::MadSkills(),       //
        Pistol::IncreasedFocus(),  //
        Pistol::RapidGetaway(),    //
        Pistol::StraightShooter(), //

        // Shotgun [done]
        Shotgun::DeadOnTarget(),     //
        Shotgun::BodyPiercing(),     //
        Shotgun::StrikeForce(),      //
        Shotgun::RapidReload(),      //
        Shotgun::InvasiveMeasures(), //

        // Rifle [done]
        Rifle::Lethality(),             //
        Rifle::ImprovedBursts(),        //
        Rifle::Accuracy(),              //
        Rifle::CallYourShots(),         //
        Rifle::ShortControlledBursts(), //
        Rifle::EagleEye(),              //
                                        // TODO: No Contest

        // Blade [done]
        Blade::TwistTheKnife(),        //
        Blade::SuddenReturn(),         //
        Blade::FortunateStrike(),      //
        Blade::FatalFlourish(),        //
        Blade::SevenAndAHalfSamurai(), //
        Blade::SharpBlades(),          //


        // Hammer [done]
        Hammer::Thunderstruck(),   //
        Hammer::Brawler(),         //
        Hammer::ImprovedStrikes(), //
        Hammer::KnuckleDown(),     //
        Hammer::Punishment(),      //
        Hammer::Tenderising(),     //
        Hammer::WreckingCrew(),    //
        Hammer::HardImpact(),      //
                                   // TODO: Street Fighter?
        // Fist [done]
        Fist::Predator(), //
        Fist::Ferocity(), //
        // TODO: One-Two passive?

        // Blood [done]
        Blood::IronMaiden(),       //
        Blood::ThickerThanWater(), //
        Blood::Doom(),             //
        Blood::ThirdDegree(),      //
                                   // TODO: Blood Bank
                                   // TODO: Toxic Affinity
                                   // TODO: Unholy Knowledge

        // Chaos [done]
        Chaos::Gnosis(),     //
        Chaos::ChaosAdept(), //
        Chaos::FeverPitch(), //
                             // TODO: Ripple Effect
                             // TODO: Cutting Thoughts
                             // TODO: Entropy

        // Elemental [done]
        Elemental::ElementalForce(),     //
        Elemental::LiveWire(),           //
        Elemental::ViolentStrikes(),     //
        Elemental::ElementalPrecision(), //
                                         // TODO: Molecular Exploitation
                                         // TODO: Highly Strung?
                                         // TODO: Aidelon
                                         // TODO: Anima Charge
                                         // TODO: Rapid Combustion?
    };
}

std::vector<Passive> Passives::Kickback::all()
{
    return{
        CritOnCritPurple(), //
        CritOnPenPurple(),  //
        CritOnCritBlue(),   //
        CritOnPenBlue(),    //

        CritPowerOnCritPurple(), //
        CritPowerOnPenPurple(),  //
        CritPowerOnCritBlue(),   //
        CritPowerOnPenBlue(),    //

        PenOnCritPurple(), //
        PenOnPenPurple(),  //
        PenOnCritBlue(),   //
        PenOnPenBlue(),    //
    };
}
std::vector<Passive> Passives::Kickback::allBlue()
{
    return{
        CritOnCritBlue(),   //
        CritOnPenBlue(),    //

        CritPowerOnCritBlue(),   //
        CritPowerOnPenBlue(),    //

        PenOnCritBlue(),   //
        PenOnPenBlue(),    //
    };
}
std::vector<Passive> Passives::Kickback::allPurple()
{
    return{
        CritOnCritPurple(), //
        CritOnPenPurple(),  //

        CritPowerOnCritPurple(), //
        CritPowerOnPenPurple(),  //

        PenOnCritPurple(), //
        PenOnPenPurple(),  //
    };
}
