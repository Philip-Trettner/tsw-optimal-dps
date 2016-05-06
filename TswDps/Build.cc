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
    std::cout << "BUILD DUMP" << std::endl;

    std::cout << "Rotation: " << std::endl;
    for (auto i = 0u; i < rotation.size(); ++i)
    {
        std::cout << "  - " << rotation[i].name << " [" << augments[i].name << "]" << std::endl;
    }
}

void Build::woodcutterDetailed()
{
    std::cout << "Woodcutter Experiment" << std::endl;
    for (auto pen = 0; pen <= 100; ++pen)
    {
        auto wcChance = woodcutterPenChance(pen / 100.f);
        std::cout << "  " << pen << "%: " << std::endl;
        
    }
}

float Build::woodcutterPenChance(float penChance)
{
    
}
