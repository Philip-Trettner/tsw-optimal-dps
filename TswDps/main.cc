#include <iostream>
#include <algorithm>
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


enum class ExploreType
{
    Best,
    Burst,
    Dummy,
};

void explore(ExploreType type);

int main(int argc, char *argv[])
{
    SkillTable::loadSkillTable(pathOf(__FILE__) + "/Skill Scaling - Scalings.tsv");

    (void)argc;
    (void)argv;

    /**
     * TODO:
     *
     * * Bullet Ballet
     * * Subway Tokens (.4 and .9)
     *
     * later: afflictions + signet of corruption
     *
     * Doom is strange... is consumed on focus/burst builders?
     */

    Optimizer optimizer;

    const auto exploreType = ExploreType::Dummy;
    const bool exploration = true;
    const bool optimization = true;

    const bool dpsTest = true;

    const int maxTime = 100 * 1000 * 60;
    const int burstFight = 20 * 60;
    const bool longRun = true;
    const bool buffs = true;

    optimizer.excludeSkillsAndPassives = {
        //"Power Line",         //
        "Live Wire",          //
        "Sudden Return",      //
        "One In The Chamber", //
        "Fortunate Strike",   //
        "Thunderstruck",      //
    };
    // no procs
    /*optimizer.excludeSkillsAndPassives = {
        "Live Wire",          //
        "Sudden Return",      //
        "One In The Chamber", //
        "Fortunate Strike",   //
        "Thunderstruck",      //
    };*/

    // for (auto const& s : g.enumerateGearStats({Rating::Crit,
    // Rating::CritPower}, true))
    //    std::cout << s.critRating << ";" << s.critPowerRating << std::endl;


    if (exploration)
    {
        explore(exploreType);
        return 0;
    }


    AggregateLog log;
    VerboseLog vlog;
    vlog.skillsOnly = !true;
    vlog.logResources = !!true;
    StatLog slog;
    if (!dpsTest)
        log.logs.push_back(&vlog);
    log.logs.push_back(&slog);
    Simulation &s = optimizer.refSim;
    s.log = &log;

    if (longRun && (dpsTest || optimization))
        s.enemyInfo.allVulnerabilities = true;

    if (!buffs)
        s.buffAt = INF_TIME;

    // s.loadBuild(Builds::fromFile(pathOf(__FILE__) + "/results/best/Elemental-Hammer.json"));
    // s.loadBuild(Builds::fromFile(pathOf(__FILE__) + "/results/best/Elemental-Blood.json"));
    s.loadBuild(Builds::currTest());
    // s.loadBuild(Builds::currMaxFistHammer());
    // s.loadBuild(Builds::currMaxPistolShotgun());
    // s.loadBuild(Builds::procHairtriggerOnly());
    // s.loadBuild(Builds::procBurstChaosRifle());
    // s.loadBuild(Builds::procBurstChaosFist());
    // s.loadBuild(Builds::hammerTest());

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

    std::cout << "Build:" << std::endl;
    s.build().shortDump();
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

    enum class ExploreType
    {
        Best,
        Burst
    };
}

void explore(ExploreType type)
{
    // path
    auto suffix = "INVALID";
    switch (type)
    {
    case ExploreType::Best:
        suffix = "best";
        break;
    case ExploreType::Burst:
        suffix = "burst";
        break;
    case ExploreType::Dummy:
        suffix = "dummy";
        break;
    }

    std::cout << "Exploring '" << suffix << "'" << std::endl;

    auto weapons = {Weapon::Blade,     Weapon::Fist,  Weapon::Hammer, Weapon::Blood,  Weapon::Chaos,
                    Weapon::Elemental, Weapon::Rifle, Weapon::Pistol, Weapon::Shotgun};
    auto resPath = pathOf(__FILE__) + "/results/";
    std::map<Weapon, std::map<Weapon, double>> w2w2dps;
    std::map<std::string, int> passiveCnt;
    int buildCnt = 0;
    for (auto w1 : weapons)
        for (auto w2 : weapons)
        {
            if (w2 >= w1)
                break;

            auto bestpath = resPath + suffix + "/" + to_string(w1) + "-" + to_string(w2) + ".json";

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

            switch (type)
            {
            case ExploreType::Best:
                s.enemyInfo.allVulnerabilities = true;
                o.timePerFight = 10 * 60 * 60; // 10 min
                break;
            case ExploreType::Burst:
                s.enemyInfo.allVulnerabilities = true;
                o.timePerFight = 15 * 60; // 15s
                break;
            case ExploreType::Dummy:
                s.enemyInfo.allVulnerabilities = false; // no vuln
                o.timePerFight = 2 * 60 * 60;           // 2 min parses
                s.buffAt = INF_TIME;                    // no buffs
                break;
            }

            std::cout << "TESTING " << to_string(w1) << " and " << to_string(w2) << (cont ? " [cont.]" : "") << std::flush;
            o.run(5);

            auto const &builds = o.getTopBuilds();
            auto maxDPS = builds[0].first;
            w2w2dps[w1][w2] = maxDPS;
            w2w2dps[w2][w1] = maxDPS;

            std::cout << " ... " << maxDPS << std::endl;

            // stats
            ++buildCnt;
            for (auto const &p : builds[0].second.skills.passives)
                passiveCnt[p.name]++;

            // save
            {
                std::ofstream bfile(bestpath);
                bfile << builds[0].second.toJson().json() << std::endl;
            }
        }

    std::ofstream table(resPath + "summary-" + suffix + ".csv");
    table << "Wep-to-Wep";
    for (auto w : weapons)
        table << "," << to_string(w);
    table << std::endl;
    for (auto w1 : weapons)
    {
        table << to_string(w1);
        for (auto w2 : weapons)
        {
            if (w1 == w2)
                table << ",-";
            else
                table << "," << w2w2dps[w1][w2];
        }
        table << std::endl;
    }
    table << std::endl;
    table << "Passives:," << buildCnt << ",builds" << std::endl;
    std::vector<std::pair<int, std::string>> cnts;
    for (auto const &kvp : passiveCnt)
        cnts.push_back(std::make_pair(-kvp.second, kvp.first));
    sort(begin(cnts), end(cnts));
    for (auto const &kvp : cnts)
        table << kvp.second << "," << -kvp.first << std::endl;
}
