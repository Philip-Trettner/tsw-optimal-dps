#include <iostream>

#include "Signets.hh"
#include "Simulation.hh"
#include "Build.hh"
#include "Gear.hh"
#include "CombatLog.hh"
#include "Skills.hh"
#include "Augments.hh"
#include "Passives.hh"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    // for (auto const& s : g.enumerateGearStats({Rating::Crit, Rating::CritPower}, true))
    //    std::cout << s.critRating << ";" << s.critPowerRating << std::endl;

    VerboseLog log;
    Simulation s;
    s.log = &log;

    s.skills = {
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
          Augments::Accurate(),
        },
        { // passives
          Passives::Hammer::Brawler(),
          Passives::Shotgun::DeadOnTarget(),
        }
    };

    s.rotation = FixedRotation::create({0, 1, 0, 0, 0, 0, 1, 2});

    auto& g = s.gear;

    g.setGear(PrimaryStat::Attack, Gear::TalismanQuality::QL11);

    g.pieces[Gear::Head].fix(Rating::Hit);
    g.pieces[Gear::MajorLeft].fix(Rating::Pen);
    g.pieces[Gear::MajorRight].fix(Rating::Pen);
    g.pieces[Gear::MinorLeft].fix(Rating::Hit);

    g.pieces[Gear::MajorMid].fix(Rating::CritPower);
    g.pieces[Gear::MinorMid].fix(Rating::Crit);
    g.pieces[Gear::MinorRight].fix(Rating::Crit);

    g.pieces[Gear::MajorLeft].signet = Signets::Major::Violence();
    g.pieces[Gear::MajorMid].signet = Signets::Major::Violence();
    g.pieces[Gear::MajorRight].signet = Signets::Major::Violence();

    g.pieces[Gear::MinorLeft].signet = Signets::Minor::Issue1p5();
    g.pieces[Gear::MinorMid].signet = Signets::Minor::Issue1p5();
    g.pieces[Gear::MinorRight].signet = Signets::Minor::Issue1p5();

    g.leftWeapon = Weapon::Pistol;
    g.pieces[Gear::WeaponLeft].fix(Rating::Crit);
    g.rightWeapon = Weapon::Shotgun;
    g.pieces[Gear::WeaponRight].fix(Rating::Crit);

    std::cout << "Base gear stats:" << std::endl;
    g.gearStats().dumpDpsStats();
    std::cout << std::endl;

    s.init();

    std::cout << "Skill stats:" << std::endl;
    s.dumpSkillStats();
    std::cout << std::endl;

    s.simulate(20 * 60);
}
