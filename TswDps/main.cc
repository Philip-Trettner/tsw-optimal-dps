#include <iostream>

#include "Augments.hh"
#include "Build.hh"
#include "CombatLog.hh"
#include "Gear.hh"
#include "Passives.hh"
#include "Signets.hh"
#include "Simulation.hh"
#include "Skills.hh"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    const bool dpsTest = true;

    // for (auto const& s : g.enumerateGearStats({Rating::Crit,
    // Rating::CritPower}, true))
    //    std::cout << s.critRating << ";" << s.critPowerRating << std::endl;

    AggregateLog log;
    VerboseLog vlog;
    StatLog slog;
    if (!dpsTest)
        log.logs.push_back(&vlog);
    log.logs.push_back(&slog);
    Simulation s;
    s.log = &log;

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
                    Augments::Ferocious(), //
                    Augments::Grievous(),  //
                    Augments::Accurate(),  //
                },
                {
                    // passives
                    Passives::Hammer::Brawler(),           //
                    Passives::Blood::IronMaiden(),         //
                    Passives::Elemental::ElementalForce(), //
                    Passives::Pistol::OneInTheChamber(),   //
                    Passives::Blade::SuddenReturn(),       //
                    Passives::Blade::FortunateStrike(),    //
                    Passives::Hammer::Thunderstruck(),     //
                }};

    // s.rotation = FixedRotation::create({0, 1, 0, 0, 0, 0, 1, 2});
    s.rotation = FixedRotation::create({0});

    auto &g = s.gear;

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

    if (dpsTest)
    {
        s.lowVarianceMode = true;
        while (s.totalTimeAccum < 500 * 1000 * 60)
            s.simulate(20 * 60);
        s.dumpBriefReport();
    }
    else
    {
        s.simulate(20 * 60);
        std::cout << std::endl;
        s.dumpBriefReport();
    }

    std::cout << std::endl;
    slog.dump(&s);
}
