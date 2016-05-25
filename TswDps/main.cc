#include <iostream>

#include "Augments.hh"
#include "Build.hh"
#include "CombatLog.hh"
#include "Gear.hh"
#include "Passives.hh"
#include "Signets.hh"
#include "Simulation.hh"
#include "Skills.hh"
#include "Builds.hh"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    const bool dpsTest = true;

    const int burstFight = 13 * 60;
    const bool longRun = true;
    const bool buffs = true;

    // for (auto const& s : g.enumerateGearStats({Rating::Crit,
    // Rating::CritPower}, true))
    //    std::cout << s.critRating << ";" << s.critPowerRating << std::endl;

    AggregateLog log;
    VerboseLog vlog;
    vlog.skillsOnly = !true;
    StatLog slog;
    if (!dpsTest)
        log.logs.push_back(&vlog);
    log.logs.push_back(&slog);
    Simulation s;
    s.log = &log;

    if (longRun && dpsTest)
        s.enemyInfo.allVulnerabilities = true;

    if (!buffs)
        s.buffAt = 100000;

    Builds::procHairtriggerOnly(s);
    // Builds::procBurstChaosRifle(s);
    // Builds::procBurstChaosFist(s);
    // Builds::hammerTest(s);

    auto &g = s.gear;

    // stats
    g.setGear(PrimaryStat::Attack, Gear::TalismanQuality::QL11);

    g.pieces[Gear::Head].fix(Rating::Hit);
    g.pieces[Gear::MajorLeft].fix(Rating::Pen);
    g.pieces[Gear::MajorMid].fix(Rating::CritPower); // WC
    g.pieces[Gear::MajorMid].set(PrimaryStat::Attack, Gear::TalismanQuality::QL10_9);
    g.pieces[Gear::MajorRight].fix(Rating::Pen);
    g.pieces[Gear::MinorLeft].fix(Rating::Hit);

    g.pieces[Gear::MinorMid].free(Rating::Crit);
    g.pieces[Gear::MinorRight].free(Rating::Crit);

    // signets
    g.pieces[Gear::Head].signet = Signets::HeadWeapon::Laceration();

    g.pieces[Gear::MajorLeft].signet = Signets::Major::Violence();
    g.pieces[Gear::MajorMid].signet = Signets::Major::WoodcuttersWrath();
    g.pieces[Gear::MajorRight].signet = Signets::Major::Violence();

    g.pieces[Gear::MinorLeft].signet = Signets::Minor::Issue1p5();
    g.pieces[Gear::MinorMid].signet = Signets::Minor::Issue1p5();
    g.pieces[Gear::MinorRight].signet = Signets::Minor::Issue1p5();

    g.pieces[Gear::WeaponLeft].free(Rating::Crit);
    g.pieces[Gear::WeaponRight].free(Rating::Crit);

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

    const int maxTime = 100 * 1000 * 60;

    if (dpsTest)
    {
        s.lowVarianceMode = true;
        if (longRun)
            s.simulate(100 * 1000 * 60);
        else
            while (s.totalTimeAccum < maxTime)
                s.simulate(burstFight);
        s.dumpBriefReport();
    }
    else
    {
        s.simulate(burstFight);
        std::cout << std::endl;
        s.dumpBriefReport();
    }

    std::cout << std::endl;
    slog.dump(&s);

    if (dpsTest)
    {
        std::cout << std::endl;
        s.analyzePassiveContribution(maxTime);
    }
}
