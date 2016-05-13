#include <iostream>

#include "Signets.hh"
#include "Simulation.hh"
#include "Build.hh"
#include "Gear.hh"
#include "CombatLog.hh"
#include "Skills.hh"
#include "Augments.hh"
#include "Passives.hh"

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    // for (auto const& s : g.enumerateGearStats({Rating::Crit, Rating::CritPower}, true))
    //    std::cout << s.critRating << ";" << s.critPowerRating << std::endl;

    // CombatLog log;
    VerboseLog log;
    Simulation s;
    //s.log = &log;

    s.skills = {{
                    // skills
                    Skills::Pistol::HairTrigger(),  //
                    Skills::Shotgun::OutForAKill(), //
                    Skills::Pistol::Shootout(),     //
                    Skills::Shotgun::Bombardment(), //
                    Skills::Shotgun::Striker(),     //
                },
                {
                    // augs
                    Augments::Brutal(),    //
                    Augments::Piercing(),  //
                    Augments::Fierce(), //
                    Augments::Grievous(),    //
                    Augments::Accurate(),  //
                },
                {
                    // passives
                    Passives::Hammer::Brawler(),       //
                    Passives::Shotgun::DeadOnTarget(), //
                    Passives::Blood::IronMaiden(),     //
                    Passives::Blade::TwistTheKnife(),  //
                    Passives::Rifle::Lethality(),      //
                    Passives::Pistol::SealTheDeal(),   //
                }};

    s.rotation = FixedRotation::create({4, 1, 4, 4, 4, 4, 1, 2});

    auto& g = s.gear;

    // stats
    g.setGear(PrimaryStat::Attack, Gear::TalismanQuality::QL11);

    g.pieces[Gear::Head].fix(Rating::Hit);
    g.pieces[Gear::MajorLeft].fix(Rating::Pen);
    g.pieces[Gear::MajorRight].fix(Rating::Pen);
    g.pieces[Gear::MinorLeft].fix(Rating::Hit);

    g.pieces[Gear::MajorMid].fix(Rating::CritPower);
    g.pieces[Gear::MinorMid].fix(Rating::Crit);
    g.pieces[Gear::MinorRight].fix(Rating::Crit);

    // signets
    g.pieces[Gear::Head].signet = Signets::HeadWeapon::Laceration();

    g.pieces[Gear::MajorLeft].signet = Signets::Major::Violence();
    g.pieces[Gear::MajorMid].signet = Signets::Major::Violence();
    g.pieces[Gear::MajorRight].signet = Signets::Major::Violence();

    g.pieces[Gear::MinorLeft].signet = Signets::Minor::Issue1p5();
    g.pieces[Gear::MinorMid].signet = Signets::Minor::Issue1p5();
    g.pieces[Gear::MinorRight].signet = Signets::Minor::Issue1p5();

    // weapons
    g.leftWeapon = Weapon::Pistol;
    g.pieces[Gear::WeaponLeft].fix(Rating::Crit);
    g.pieces[Gear::WeaponLeft].signet = Signets::HeadWeapon::Aggression();
    g.rightWeapon = Weapon::Shotgun;
    g.pieces[Gear::WeaponRight].fix(Rating::Crit);
    g.pieces[Gear::WeaponRight].signet = Signets::HeadWeapon::Abuse();

    std::cout << "Base gear stats:" << std::endl;
    g.gearStats().dumpDpsStats();
    std::cout << std::endl;

    s.init();

    std::cout << "Skill stats:" << std::endl;
    s.dumpSkillStats();
    std::cout << std::endl;

    /*if (false)
    {
        s.lowVarianceMode = false;
        std::cout << "normal:" << std::endl;
        for (int r = 0; r < 10; ++r)
        {
            s.resetStats();
            for (int i = 0; i < 10000; ++i)
                s.simulate(20 * 60);
            std::cout << "dps: " <<  s.totalDPS() << std::endl;
        }
        std::cout << std::endl;

        std::cout << "low variance:" << std::endl;
        s.lowVarianceMode = true;
        for (int r = 0; r < 10; ++r)
        {
            s.resetStats();
            for (int i = 0; i < 10000; ++i)
                s.simulate(20 * 60);
            std::cout << "dps: " <<  s.totalDPS() << std::endl;
        }
        std::cout << std::endl;
    }*/

    s.lowVarianceMode = true;
    while (s.totalTimeAccum < 1000 * 1000 * 60)
        s.simulate(5 * 60 * 60);
    s.dumpBriefReport();

    //s.simulate(20 * 60);
    //std::cout << std::endl;
    //s.dumpBriefReport();
}
