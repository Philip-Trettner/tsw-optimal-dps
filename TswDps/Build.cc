#include "Build.hh"

#include "Skills.hh"
#include "Passives.hh"
#include "Augments.hh"

Build::Build()
{
}

void Build::loadPistolShotgunHairTrigger()
{
    skills = {
        { // skills
          Skills::Pistol::HairTrigger(),
          Skills::Shotgun::OutForAKill(),
          Skills::Pistol::Shootout(),
          Skills::Shotgun::Bombardment(),
        },
        { // augs
          Augments::Brutal(),
          Augments::Piercing(),
          Augments::Ferocious(),
          Augments::Fierce(),
        },
        { // passives
          Passives::Hammer::Brawler(),
        }
    };

    rotation = {
        0, 1, 0, 0, 0, 0, 1, 2
    };
}

void Build::dump()
{
    std::cout << "BUILD DUMP" << std::endl;

    std::cout << "Rotation: " << std::endl;
    for (auto i = 0u; i < rotation.size(); ++i)
    {
        std::cout << "  - " << skills.skills[rotation[i]].name << " [" << skills.augments[rotation[i]].name << "]" << std::endl;
    }
}

void Build::woodcutterDetailed()
{
    std::cout << "Woodcutter Experiment" << std::endl;
    for (auto pen = 0; pen <= 100; ++pen)
    {
        auto wcChance = woodcutterPenChance(pen / 100.f);
        std::cout << "  " << pen << "%: +" << wcChance * 100.f - pen << "%" << std::endl;
        
    }
}

float Build::woodcutterPenChance(float penChance)
{
   (void)penChance;
    return -1; // TODO
}
