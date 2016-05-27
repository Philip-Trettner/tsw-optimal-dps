#include "Passives.hh"


std::vector<Passive> Passives::all()
{
    return {
        Pistol::SealTheDeal(),     //
        Pistol::OneInTheChamber(), //

        Shotgun::DeadOnTarget(), //

        Rifle::Lethality(), //

        Blade::TwistTheKnife(),   //
        Blade::SuddenReturn(),    //
        Blade::FortunateStrike(), //

        Hammer::Thunderstruck(), //
        Hammer::Brawler(),       //

        Blood::IronMaiden(), //

        Chaos::Gnosis(), //

        Elemental::ElementalForce(), //
        Elemental::LiveWire(),       //
    };
}
