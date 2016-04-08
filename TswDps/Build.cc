#include "Build.hh"

#include "Skills.hh"
#include "Augments.hh"

Build::Build()
{
}

void Build::loadPistolShotgunHairTrigger()
{
    rotation = {
        Skills::HairTrigger(),  //
        Skills::OutForAKill(1), //
        Skills::HairTrigger(),  //
        Skills::HairTrigger(),  //
        Skills::HairTrigger(),  //
        Skills::HairTrigger(),  //
        Skills::OutForAKill(4), //
        Skills::Shootout(),     //
    };
    augments = {
        Augments::Brutal(),    //
        Augments::Fierce(),    //
        Augments::Brutal(),    //
        Augments::Brutal(),    //
        Augments::Brutal(),    //
        Augments::Brutal(),    //
        Augments::Fierce(),    //
        Augments::Ferocious(), //
    };
}

void Build::dump()
{

}
