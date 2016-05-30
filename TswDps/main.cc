#include <iostream>

#include <fstream>

#include "Augments.hh"
#include "Build.hh"
#include "CombatLog.hh"
#include "Gear.hh"
#include "Passives.hh"
#include "Signets.hh"
#include "Simulation.hh"
#include "Skills.hh"
#include "Builds.hh"
#include "SkillTable.hh"
#include "Optimizer.hh"

void explore();

int main(int argc, char *argv[])
{
    SkillTable::loadSkillTable(pathOf(__FILE__) + "/Skill Scaling - Scalings.tsv");

    (void)argc;
    (void)argv;

    /**
     * TODO:
     *
     * * Anima Charge
     * * Doom
     * * Bullet Ballet
     * * FtM, IM
     * * Subway Tokens (.4 and .9)
     *
     * later: afflictions + signet of corruption
     */

    const bool exploration = true;
    const bool optimization = true;

    const bool dpsTest = false;

    const int maxTime = 100 * 1000 * 60;
    const int burstFight = 20 * 60;
    const bool longRun = true;
    const bool buffs = true;

    // for (auto const& s : g.enumerateGearStats({Rating::Crit,
    // Rating::CritPower}, true))
    //    std::cout << s.critRating << ";" << s.critPowerRating << std::endl;


    if (exploration)
    {
        explore();
        return 0;
    }


    Optimizer optimizer;

    AggregateLog log;
    VerboseLog vlog;
    vlog.skillsOnly = !true;
    vlog.logResources = !true;
    StatLog slog;
    if (!dpsTest)
        log.logs.push_back(&vlog);
    log.logs.push_back(&slog);
    Simulation &s = optimizer.refSim;
    s.log = &log;

    if (longRun && (dpsTest || optimization))
        s.enemyInfo.allVulnerabilities = true;

    if (!buffs)
        s.buffAt = 100000;

    s.loadBuild(Builds::currTest());
    // s.loadBuild(Builds::currMaxFistHammer());
    // s.loadBuild(Builds::currMaxPistolShotgun());
    // s.loadBuild(Builds::procHairtriggerOnly());
    // s.loadBuild(Builds::procBurstChaosRifle());
    // s.loadBuild(Builds::procBurstChaosFist());
    // s.loadBuild(Builds::hammerTest());

    std::cout << "Build:" << std::endl;
    s.build().shortDump();
    std::cout << std::endl;

    std::cout << "Base gear stats:" << std::endl;
    s.gear.gearStats().dumpDpsStats();
    std::cout << std::endl;

    if (optimization)
    {
        optimizer.run();

        auto const &builds = optimizer.getTopBuilds();
        int cnt = 5;
        std::cout << std::endl;
        std::cout << "Top " << cnt << " builds:" << std::endl;
        std::cout << std::endl;
        for (auto const &kvp : builds)
        {
            std::cout << kvp.first << " DPS" << std::endl;
            kvp.second.shortDump();
            std::cout << std::endl;

            --cnt;
            if (cnt <= 0)
                break;
        }

        std::cout << std::endl;
        std::cout << "Analysis of top build:" << std::endl;
        std::cout << std::endl;
        s.log = &slog;
        s.lowVarianceMode = true;
        s.resetStats();
        s.loadBuild(builds[0].second);
        s.init();
        while (s.totalTimeAccum < maxTime)
            s.simulate(optimizer.timePerFight);
        std::cout << std::endl;
        s.dumpBriefReport();
        std::cout << std::endl;
        slog.dump(&s);
        std::cout << std::endl;
        s.analyzePassiveContribution();

        return 0;
    }

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
        if (longRun)
            s.simulate(maxTime);
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

void explore()
{
    auto weapons = {Weapon::Blade,     Weapon::Fist,  Weapon::Hammer, Weapon::Blood,  Weapon::Chaos,
                    Weapon::Elemental, Weapon::Rifle, Weapon::Pistol, Weapon::Shotgun};
    auto resPath = pathOf(__FILE__) + "/results/";
    std::ofstream table(resPath + "summary.csv");
    table << "Wep-to-Wep";
    for (auto w : weapons)
        if (w != *(weapons.begin() + (weapons.size() - 1)))
            table << " " << to_string(w);
    table << std::endl;
    for (auto w1 : weapons)
    {
        if (w1 == *begin(weapons))
            continue;

        table << to_string(w1);
        for (auto w2 : weapons)
        {
            if (w2 >= w1)
                break;

            auto bestpath = resPath + "best/" + to_string(w1) + "-" + to_string(w2) + ".json";

            // start build
            Build b;
            b.rotation = DefaultRotation::create();
            b.gear.loadStandardDpsGear();
            b.gear.leftWeapon = w1;
            b.gear.rightWeapon = w2;
            // or continue prev best
            bool cont = false;
            {
                std::ifstream bfile(bestpath);
                if (bfile.good())
                {
                    jsonxx::Object o;
                    if (o.parse(bfile))
                    {
                        b.fromJson(o);
                        cont = true;
                    }
                }
            }

            // optimize
            Optimizer o;
            o.silent = true;
            auto &s = o.refSim;
            s.loadBuild(b);
            s.enemyInfo.allVulnerabilities = true;

            std::cout << "TESTING " << to_string(w1) << " and " << to_string(w2) << (cont ? " [cont.]" : "") << std::endl;
            o.run(100);

            auto const &builds = o.getTopBuilds();
            auto maxDPS = builds[0].first;
            table << " " << maxDPS;

            {
                std::ofstream bfile(bestpath);
                bfile << builds[0].second.toJson().json() << std::endl;
            }
        }
        table << std::endl;
    }
}
