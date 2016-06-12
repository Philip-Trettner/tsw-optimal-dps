#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>

#include <gtest/gtest.h>

#include <aion/Action.hh>
#include <aion/ActionAnalyzer.hh>

#include "Augments.hh"
#include "Build.hh"
#include "Builds.hh"
#include "CombatLog.hh"
#include "Gear.hh"
#include "Optimizer.hh"
#include "Passives.hh"
#include "Signets.hh"
#include "Simulation.hh"
#include "SkillTable.hh"
#include "Skills.hh"

#define DEPLOY 0

enum class ExploreType
{
    Best,
    Burst,
    Dummy,
};

void explore(ExploreType type, double timeMult);

void debugRun()
{
    /**
     * TODO:
     *
     * * Team Mercurials
     * * better gear optimization (total reglyph mutation)
     * * check if laceration on head makes a difference => make table with signet variations (head, builder, 2ndary)
     * * .5 glyphs
     * * "Budget" raid scenario
     * * Randomize DABS more to reduce aliasing
     * * Fix four horsemen
     * * Third Degree
     *
     * later: afflictions + signet of corruption
     *
     * Doom is strange... is consumed on focus/burst builders?
     *
     * Known Issues:
     *  * Double Up is not logged
     *  * Calamity is not logged
     */

    Optimizer optimizer;

    const bool continuousExploration = true;
    const double longerTimePerRun = 1.1;
    const auto exploreType = ExploreType::Best;
    const bool exploration = !true;
    const bool optimization = !true;

    const bool dpsTest = !true;
    const bool varianceComparison = !true;

    const int maxTime = 100 * 1000 * 60;
    const int burstFight = 20 * 60;
    const bool longRun = true;
    const bool buffs = true;

    optimizer.excludeSkillsAndPassives = {
        //"Power Line", //
        /*"Live Wire",          //
        "Sudden Return",      //
        "One In The Chamber", //
        "Fortunate Strike",   //
        "Thunderstruck",      //*/
    };
    // no procs
    /*optimizer.excludeSkillsAndPassives = {
        "Live Wire",          //
        "Sudden Return",      //
        "One In The Chamber", //
        "Fortunate Strike",   //
        "Thunderstruck",      //
        "Gnosis",             //
    };*/


    if (exploration)
    {
        double mult = 1;
        do
        {
            explore(exploreType, mult);
            mult *= longerTimePerRun;
        } while (continuousExploration);

        return;
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
        std::map<std::string, double> relDmg;
        s.analyzeIndividualContribution(optimizer.timePerFight, maxTime, relDmg);

        return;
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
        if (varianceComparison)
        {
            s.lowVarianceMode = false;
            std::cout << "normal:" << std::endl;
            for (int r = 0; r < 10; ++r)
            {
                s.resetStats();
                for (int i = 0; i < 1000; ++i)
                    s.simulate(200 * 60);
                std::cout << "dps: " << s.totalDPS() << std::endl;
            }
            std::cout << std::endl;

            std::cout << "low variance:" << std::endl;
            s.lowVarianceMode = true;
            for (int r = 0; r < 10; ++r)
            {
                s.resetStats();
                for (int i = 0; i < 1000; ++i)
                    s.simulate(200 * 60);
                std::cout << "dps: " << s.totalDPS() << std::endl;
            }
            std::cout << std::endl;
        }
        else
        {
            s.lowVarianceMode = true;
            if (longRun)
                s.simulate(maxTime);
            else
                while (s.totalTimeAccum < maxTime)
                    s.simulate(burstFight);
            s.dumpBriefReport();
        }
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
        std::map<std::string, double> relDmg;
        s.analyzeIndividualContribution(longRun ? maxTime : burstFight, maxTime, relDmg);
    }
}

void explore(ExploreType type, double timeMult)
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
            o.excludeSkillsAndPassives = {
              //  "Power Line" // DEBUG
            };
            o.timePerTest = (int)(o.timePerTest * timeMult);
            o.silent = true;
            auto &s = o.refSim;
            s.loadBuild(b);

            o.threadOverwrite = 4; // DEBUG!

            switch (type)
            {
            case ExploreType::Best:
                s.enemyInfo.allVulnerabilities = true;
                o.timePerFight = 2.5 * 60 * 60; // 2.5 min
                break;
            case ExploreType::Burst:
                s.enemyInfo.allVulnerabilities = true;
                o.timePerFight = 15 * 60; // 15s
                break;
            case ExploreType::Dummy:
                s.enemyInfo.dummySetting();
                o.timePerFight = 2 * 60 * 60; // 2 min parses
                s.buffAt = INF_TIME;          // no buffs
                break;
            }

            std::cout << "TESTING " << to_string(w1) << " and " << to_string(w2) << (cont ? " [cont.]" : "") << std::flush;
            o.run(10);

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

int main(int argc, char *argv[])
{
#if DEPLOY
    SkillTable::loadSkillTable("Skill Scaling - Scalings.tsv");
#else
    SkillTable::loadSkillTable(pathOf(__FILE__) + "/Skill Scaling - Scalings.tsv");
#endif

    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("ArtificialMind's Combat Simulator");
    QCoreApplication::setApplicationVersion("1.0");

    // ==========================================================================
    // setup otions
    QCommandLineParser parser;
    parser.setApplicationDescription(app.applicationName());
    parser.addHelpOption();
    parser.addVersionOption();

    // .. build
    parser.addPositionalArgument("build", "Build (either a .json file or a 'Weapon+Weapon' combination)");

    // .. debug
    QCommandLineOption oDebug({"d", "debug"}, "Debug Procedure");
#if !DEPLOY
    parser.addOption(oDebug);
#endif

    // .. log level
    QCommandLineOption oLog(
        {"l", "log"}, "Log Level (0 = none, 1 = stats [default], 2 = skills, 3 = hits, 4 = effects, 5 = all)", "level", "1");
    parser.addOption(oLog);

    // .. analysis
    QCommandLineOption oAnalysis({"a", "analysis"},
                                 "Instead of simulating, analyzes the dps impact of every part of the build.");
    parser.addOption(oAnalysis);

    // .. fight scenario
    QCommandLineOption oFight({"f", "fight"}, "Fight scenario ('raid' [default], 'burst', 'dummy', 'raid-melee', "
                                              "'raid-ranged', 'raid-magic', or file)",
                              "scenario", "raid");
    parser.addOption(oFight);

    // .. combat time
    QCommandLineOption oTime({"t", "time"},
                             "Simulated total combat time in seconds (default is taken from fight scenario)", "time");
    parser.addOption(oTime);
    QCommandLineOption oFightTime("fight-time", "Simulated fight time in seconds (default is taken from fight "
                                                "scenario), affects optimizer and analyser",
                                  "time");
    parser.addOption(oFightTime);
    QCommandLineOption oOptTime("optimize-time", "Simulation time per evaluation in the optimizer (default 2h)", "time", "2h");
    parser.addOption(oOptTime);
    QCommandLineOption oAnaTime("analysis-time", "Simulation time per evaluation in the analyzer (default 8h)", "time", "8h");
    parser.addOption(oAnaTime);

    // .. optimizer
    QCommandLineOption oOptimize({"o", "optimize"}, "Optimizes the DPS of the given build for a given number of rounds "
                                                    "(settings are included in the fight scenario)",
                                 "rounds");
    parser.addOption(oOptimize);

    // .. dump build at end
    QCommandLineOption oDumpBuild(
        "dump-build", "Dumps the build (as json) at the end (affected by optimizer), use '.' for console output",
        "file");
    parser.addOption(oDumpBuild);
    QCommandLineOption oUpdate(
        {"u", "update"}, "Updates the loaded build (if json) at the end. Same as --dump-build with the same build");
    parser.addOption(oUpdate);

    // .. dump scenario at end
    QCommandLineOption oDumpScenario("dump-scenario", "Dumps the fight scenario (as json), use '.' for console output", "file");
    parser.addOption(oDumpScenario);

    // .. output base stats per skill
    QCommandLineOption oSkillStats("skill-stats", "Outputs base stats for each skill");
    parser.addOption(oSkillStats);

    // .. shows top X builds after optimization
    QCommandLineOption oShowTop("show-top", "Prints to [N] builds after optimization.", "N");
    parser.addOption(oShowTop);

    // .. threads for optimization
    QCommandLineOption oThreads("threads", "Overwrites the number of threads used in optimization (default: # cores).", "N");
    parser.addOption(oThreads);

    // .. builds per gen
    QCommandLineOption oBuildsPerGen("builds-per-gen",
                                     "Number of new potential builds per optimizer generation (default: 60).", "N");
    parser.addOption(oBuildsPerGen);

    // .. fast-opt
    QCommandLineOption oFastOpt(
        "fast-opt", "Overwrite some optimization settings for speed (but less accuracy). Useful when starting out.");
    parser.addOption(oFastOpt);

    // .. starting build
    QCommandLineOption oStart("start", "Starting build in 'Weapon+Weapon' form, used if file does not exist.", "build");
    parser.addOption(oStart);

    // .. starting build
    QCommandLineOption oBBCode("bb", "Writes BBCode for the current (analyzed) build into a given file (or '.' for "
                                     "console). Affected by analysis time, implies -a.",
                               "file");
    parser.addOption(oBBCode);

    // .. test
    QCommandLineOption oTest("test", "Runs internal tests.");
#if !DEPLOY
    parser.addOption(oTest);
    parser.addOption({"gtest_filter", "test filter", "filter"});
#endif

    // .. timings
    QCommandLineOption oTimings("timings", "Dump full internal timings.");
    parser.addOption(oTimings);

    // ==========================================================================
    // parse options
    parser.process(app);

    // ==========================================================================
    // process options

    Optimizer o;
    Simulation &s = o.refSim;
    int fightTime = ticksFromTimeStr("20s");
    int totalTime = fightTime;

    std::string settingName = "Unknown";

    // .. debug only
    if (parser.isSet(oDebug))
    {
        debugRun();
        return 0;
    }

    // .. tests
    if (parser.isSet(oTest))
    {
        testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }

    // args
    auto args = parser.positionalArguments();
    if (args.length() == 0)
    {
        std::cerr << "No build provided" << std::endl;
        std::cerr << std::endl;
        parser.showHelp(-1);
    }

    // .. build
    auto buildName = args[0];
    auto buildFile = buildName;
    auto buildIsFile = false;
    Build b;
    b.gear.loadEmptyDpsGear();              // for weapons and base stats
    b.rotation = DefaultRotation::create(); // default rot
    if (parser.isSet(oStart) && !std::ifstream(buildName.toStdString()).good())
    {
        buildName = parser.value(oStart);
        buildIsFile = true;
        std::cout << "Using start build " << buildName.toStdString() << std::endl;
    }
    // .. .. from weapon+weapon
    if (buildName.contains('+') && !buildName.endsWith(".json"))
    {
        Weapon w1 = Weapon::None, w2 = Weapon::None;
        auto parts = buildName.split('+');
        auto valid = true;
        if (parts.length() != 2)
            valid = false;
        else
        {
            w1 = parseWeapon(parts[0].toStdString());
            w2 = parseWeapon(parts[1].toStdString());

            if (w1 == Weapon::None || w2 == Weapon::None || w1 == w2)
                valid = false;
        }

        if (!valid)
        {
            std::cerr << "Invalid weapon name/combination '" << buildName << "'" << std::endl;
            std::cerr << std::endl;
            std::cerr << "Weapons must be different." << std::endl;
            std::cerr << std::endl;
            std::cerr << "Weapon name must be in:" << std::endl;
            std::cerr << "  Ranged: Pistol, Shotgun, Rifle" << std::endl;
            std::cerr << "  Melee:  Blade, Hammer, Fist" << std::endl;
            std::cerr << "  Magic:  Blood, Chaos, Elemental" << std::endl;
            std::cerr << std::endl;
            parser.showHelp(-1);
        }
        b.gear.leftWeapon = w1;
        b.gear.rightWeapon = w2;

        // get at least one builder
        for (auto const &s : Skills::all())
            if (s.weapon == w1 || s.weapon == w2)
                if (s.skilltype == SkillType::Builder)
                {
                    b.skills.skills[0] = s;
                    break;
                }
    }
    else if (std::ifstream(buildName.toStdString()).good())
    {
        std::ifstream file(buildName.toStdString());
        jsonxx::Object o;
        o.parse(file);
        b.fromJson(o);
        buildIsFile = true;
    }
    else
    {
        std::cerr << "Could not open file " << buildName << std::endl;
        std::cerr << std::endl;
        parser.showHelp(-1);
    }

    // .. scenario
    auto scen = parser.value(oFight);
    std::cout << "Fight Scenario: " << scen.toStdString() << std::endl;
    std::cout << std::endl;
    if (scen.toLower() == "raid")
    {
        fightTime = ticksFromTimeStr("2.5m");
        totalTime = ticksFromTimeStr("48h");
        s.enemyInfo.allVulnerabilities = true;
        s.lowVarianceMode = true;
        settingName = "Raid/Dungeon Setting";
    }
    else if (scen.toLower() == "burst")
    {
        fightTime = totalTime = ticksFromTimeStr("20s");
        s.enemyInfo.allVulnerabilities = true;
        settingName = "Burst Fight";
    }
    else if (scen.toLower() == "budget")
    {
        fightTime = ticksFromTimeStr("2.5m");
        totalTime = ticksFromTimeStr("48h");
        s.enemyInfo.allVulnerabilities = true;
        s.lowVarianceMode = true;
        settingName = "Raid/Dungeon Setting, .9.5 only, no Woodcutters, no NM Raid Drops";
    }
    else if (scen.toLower() == "dummy")
    {
        fightTime = ticksFromTimeStr("2.5m");
        totalTime = ticksFromTimeStr("48h");
        s.buffAt = INF_TIME;
        s.enemyInfo.dummySetting();
        s.lowVarianceMode = true;
        settingName = "Testchamber Dummies (sustained)";
    }
    else if (scen.toLower() == "raid-melee")
    {
        fightTime = ticksFromTimeStr("2.5m");
        totalTime = ticksFromTimeStr("48h");
        s.enemyInfo.allVulnerabilities = true;
        s.lowVarianceMode = true;
        o.forceVulnerability = DmgType::Melee;
        settingName = "Raid/Dungeon Setting with Melee Vulnerability";
    }
    else if (scen.toLower() == "raid-ranged")
    {
        fightTime = ticksFromTimeStr("2.5m");
        totalTime = ticksFromTimeStr("48h");
        s.enemyInfo.allVulnerabilities = true;
        s.lowVarianceMode = true;
        o.forceVulnerability = DmgType::Ranged;
        settingName = "Raid/Dungeon Setting with Ranged Vulnerability";
    }
    else if (scen.toLower() == "raid-magic")
    {
        fightTime = ticksFromTimeStr("2.5m");
        totalTime = ticksFromTimeStr("48h");
        s.enemyInfo.allVulnerabilities = true;
        s.lowVarianceMode = true;
        o.forceVulnerability = DmgType::Magic;
        settingName = "Raid/Dungeon Setting with Magic Vulnerability";
    }
    else if (std::ifstream(scen.toStdString()).good())
    {
        std::ifstream file(buildName.toStdString());
        jsonxx::Object o;
        o.parse(file);
        s.fightFromJson(o);

        fightTime = ticksFromJsonObj(o, "Fight Time");
        totalTime = ticksFromJsonObj(o, "Total Time");

        settingName = scen.toStdString();
    }
    else
    {
        std::cerr << "Could not open file " << scen << std::endl;
        std::cerr << std::endl;
        parser.showHelp(-1);
    }

    // .. combat time
    if (parser.isSet(oTime))
        totalTime = ticksFromTimeStr(parser.value(oTime).toStdString());
    if (parser.isSet(oFightTime))
        fightTime = ticksFromTimeStr(parser.value(oFightTime).toStdString());

    // .. optimizer
    auto optimizerRounds = 0;
    if (parser.isSet(oOptimize))
        optimizerRounds = parser.value(oOptimize).toInt();
    bool optimization = optimizerRounds > 0;

    // .. threads
    if (parser.isSet(oThreads))
        o.threadOverwrite = parser.value(oThreads).toInt();

    // .. builds per gen
    if (parser.isSet(oBuildsPerGen))
        o.newBuildsPerGen = parser.value(oBuildsPerGen).toInt();

    // .. optimizer time
    auto optTime = ticksFromTimeStr(parser.value(oOptTime).toStdString());

    // .. fast opt
    if (parser.isSet(oFastOpt))
    {
        o.newBuildsPerGen = 150;
        optTime = ticksFromTimeStr("2m");
    }

    // .. analyzer
    auto analyze = parser.isSet(oAnalysis) || parser.isSet(oBBCode);

    // .. log
    auto loglevel = parser.value(oLog).toInt();
    AggregateLog log;
    VerboseLog vlog;
    vlog.logResources = loglevel >= 5;
    vlog.logEffects = loglevel >= 4;
    vlog.skillsOnly = loglevel <= 2;
    StatLog slog;
    log.logs.push_back(&slog);
    if (loglevel >= 2)
        log.logs.push_back(&vlog);
    s.log = loglevel <= 0 ? nullptr : &log;

    // ==========================================================================
    // print info

    // initialize sim
    s.loadBuild(b);
    s.init();

    // .. print build
    std::cout << "(Initial) Build:" << std::endl;
    b.shortDump();
    std::cout << std::endl;

    // .. skill stats
    if (parser.isSet(oSkillStats))
    {
        s.dumpSkillStats();
        std::cout << std::endl;
    }

    // ==========================================================================
    // do simulation

    if (optimization)
    {
        o.timePerFight = fightTime;
        o.timePerTest = optTime;
        o.run(optimizerRounds);

        if (o.getTopBuilds().empty())
        {
            std::cout << "Error while optimizing" << std::endl;
            std::cout << std::endl;
            parser.showHelp(-1);
        }

        b = o.getTopBuilds()[0].second; // update build
        s.loadBuild(b);
    }
    else if (!analyze) // don't sim if analyze
    {
        while (s.totalTimeAccum < totalTime)
            s.simulate(fightTime);
    }

    // ==========================================================================
    // .. dump results

    // show sim results
    if (optimization)
    {
        if (parser.isSet(oShowTop))
        {
            int cnt = parser.value(oShowTop).toInt();
            std::cout << "Top " << cnt << " builds:" << std::endl;
            std::cout << std::endl;
            for (auto const &kvp : o.getTopBuilds())
            {
                std::cout << kvp.first << " DPS" << std::endl;
                kvp.second.shortDump();
                std::cout << std::endl;

                --cnt;
                if (cnt <= 0)
                    break;
            }
        }

        std::cout << "Best Optimized Build: " << o.getTopBuilds()[0].first << " DPS" << std::endl;
        b.shortDump();
        std::cout << std::endl;
    }
    else if (!analyze) // don't sim if analyze
    {
        s.dumpBriefReport();
        std::cout << std::endl;

        if (loglevel >= 1)
        {
            slog.dump(&s);
            std::cout << std::endl;
        }
    }

    // analyze
    if (analyze)
    {
        // bb code
        std::ostream *poss = nullptr;
        std::ofstream *bbs = nullptr;
        if (parser.isSet(oBBCode))
        {
            auto bbfile = parser.value(oBBCode);
            if (bbfile == ".")
                poss = &std::cout;
            else
            {
                bbs = new std::ofstream(bbfile.toStdString());
                poss = bbs;
            }
        }

        std::map<std::string, double> dmg;

        auto atime = ticksFromTimeStr(parser.value(oAnaTime).toStdString());
        s.analyzeIndividualContribution(fightTime, atime, dmg);
        std::cout << std::endl;

        // BB CODE GEN
        if (poss)
        {
            auto &oss = *poss;

            // 40000 limit!

            StatLog slog;
            auto savlog = s.log;
            s.log = &slog;
            s.init();
            s.resetStats();
            while (s.totalTimeAccum < atime)
                s.simulate(fightTime);
            s.log = savlog;

            auto const eps = .001;
            auto incBaseStr = [&](std::string const &name) -> std::string {
                if (!dmg.count(name))
                    return "";

                std::ostringstream out;
                auto reldmg = dmg.at(name);
                if (reldmg > 1 + eps)
                    out << "+" << std::fixed << std::setprecision(2) << (reldmg - 1) * 100.;
                else if (reldmg < 1 - eps)
                    out << "-" << std::fixed << std::setprecision(2) << (1 - reldmg) * 100.;
                else
                    out << "~ 0"; // below eps
                out << "% dps";

                return out.str();
            };
            auto incStr = [&](std::string const &name) -> std::string {
                if (!dmg.count(name))
                    return "";

                std::ostringstream out;
                out << "[i](" << incBaseStr(name) << ")[/i]";

                return out.str();
            };
            auto dpsAugs = Augments::allDpsAugs();
            auto augColor = [&](Augment const &a) {
                for (auto const &da : dpsAugs)
                    if (da.name == a.name)
                        return "#D32F2F";
                return "#FFEB3B";
            };
            auto signetColor = [&](Signet const &s) {
                if (s.passive.effect == EffectSlot::MothersWrathStacks)
                    return "#FFEB3B";
                return "#b82ed0";
            };

            auto builderWeapon = b.skills.skills[0].weapon;
            auto secondWeapon = b.gear.leftWeapon == builderWeapon ? b.gear.rightWeapon : b.gear.leftWeapon;
            oss << "[size=+2][color=#03A9F4]" << to_string(builderWeapon) << "[/color]/[color=#03A9F4]"
                << to_string(secondWeapon) << "[/color] (" << settingName << ", [color=#D32F2F]" << s.totalDPS()
                << " DPS[/color])[/size]" << std::endl;

            oss << std::endl;
            oss << "[size=+1][color=#B2EBF2]Build[/color][/size]" << std::endl;
            oss << "[color=#B6B6B6]" << std::endl; // grey start
            oss << "[table][tr][th]Skill[/th][th]Augment[/th][th]Passive[/th][/tr]" << std::endl;
            auto passives = b.skills.passives;
            while (passives.size() < SKILL_CNT)
                passives.push_back({});
            sort(begin(passives), end(passives),
                 [&](Passive const &l, Passive const &r) { return dmg[l.name] > dmg[r.name]; });

            for (auto i = 0; i < SKILL_CNT; ++i)
            {
                auto skillName = b.skills.skills[i].name;
                auto augName = b.skills.augments[i].name;
                auto passiveName = passives[i].name;
                if (skillName.empty() && passiveName.empty())
                    continue;

                oss << "[tr][td]"; // skill
                if (!skillName.empty())
                    oss << "[color=#03A9F4][b]" << skillName << "[/b][/color] " << incStr(skillName);
                oss << "[/td][td]"; // augment
                if (!augName.empty())
                    oss << "with [color=" << augColor(b.skills.augments[i]) << "][b]" << augName << "[/b][/color] "
                        << incStr(augName);
                oss << "[/td][td]"; // passive
                oss << "    ";
                if (!skillName.empty())
                    oss << "[color=#03A9F4][b]" << passiveName << "[/b][/color] " << incStr(passiveName);
                oss << "[/td][/tr]" << std::endl;
            }
            oss << "[/table]" << std::endl;

            oss << std::endl;
            oss << "[size=+1][color=#B2EBF2]Gear[/color][/size]" << std::endl;
            oss << "[table][tr][th]Slot[/th][th]Signet[/th][th]Notes/Alternatives[/th][/tr]" << std::endl;
            for (auto slot = Gear::Head; slot <= Gear::WeaponRight; ++slot)
            {
                auto const &piece = b.gear.pieces[slot];
                auto slotName = b.gear.pieceName(slot);
                auto signetName = piece.signet.name();
                auto seff = piece.signet.passive.effect;

                oss << "[tr][td]"; // slot
                oss << "[color=#03A9F4][b]" << slotName << "[/b][/color]";
                // oss << "[/td][td]"; // glyphs
                // oss << shortStatDump(piece.stats, false, true);
                oss << "[/td][td]"; // signet
                if (!signetName.empty())
                    oss << "with [color=" << signetColor(piece.signet) << "][b]" << signetName << "[/b][/color] "
                        << incStr(signetName);
                oss << "[/td][td]"; // notes
                if (slot == Gear::MajorMid)
                {
                    oss << " ";
                    // oss << "[color=#B6B6B6]other:[/color] ";
                    bool first = true;
                    if (seff != EffectSlot::MothersWrathStacks) // WC
                    {
                        if (first)
                            first = false;
                        else
                            oss << ", ";

                        oss << "[color=#FFEB3B][b]Woodcutters[/b][/color] " << incStr("Neck WC");
                    }
                    if (seff != EffectSlot::EgonPendant) // Egon
                    {
                        if (first)
                            first = false;
                        else
                            oss << ", ";

                        oss << "[color=#b82ed0][b].9 Egon Pendant[/b][/color] " << incStr("Neck Egon");
                    }
                    if (signetName != "Violence")
                    {
                        if (first)
                            first = false;
                        else
                            oss << ", ";

                        oss << "[color=#03A9F4][b]QL11 + Violence[/b][/color] " << incStr("Neck QL11");
                    }
                }
                else if (slot == Gear::MajorLeft && piece.signet.passive.effect == EffectSlot::ConeyIslandBand)
                {
                    oss << " [color=#03A9F4][b]QL11 + Violence[/b][/color] " << incStr("Finger QL11");
                }
                oss << "[/td][/tr]" << std::endl;
            }
            oss << "[/table]" << std::endl;
            oss << "[table][tr][th]Stats[/th][th][color=#FFEB3B][b]Hit[/b][/color][/th][th][color=#FFEB3B][b]Pen[/b][/"
                   "color][/th][th][color=#FFEB3B][b]Crit[/b][/color][/th][th][color=#FFEB3B][b]Crit "
                   "Power[/b][/color][/th][/tr]"
                << std::endl;
            auto sLeft = b.gear.gearStats() + b.gear.pieces[Gear::WeaponLeft].stats;
            auto sRight = b.gear.gearStats() + b.gear.pieces[Gear::WeaponRight].stats;
            oss << "[tr][td][color=#03A9F4][b]" << (sLeft == sRight ? "Each Weapon" : to_string(b.gear.leftWeapon))
                << "[/b][/color][/td][td]" << sLeft.hitRating << "[/td][td]" << sLeft.penRating << "[/td][td]"
                << sLeft.critRating << "[/td][td]" << sLeft.critPowerRating << "[/td][/tr]" << std::endl;
            if (sLeft != sRight)
                oss << "[tr][td][color=#03A9F4][b]" << to_string(b.gear.rightWeapon) << "[/b][/color][/td][td]"
                    << sRight.hitRating << "[/td][td]" << sRight.penRating << "[/td][td]" << sRight.critRating
                    << "[/td][td]" << sRight.critPowerRating << "[/td][/tr]" << std::endl;
            oss << "[/table]" << std::endl;

            oss << std::endl;
            oss << std::endl;
            oss << "[size=+1][color=#B2EBF2]Extras[/color][/size]" << std::endl;
            oss << "[table]" << std::endl;
            if (b.potionStats != Stats())
                oss << "[tr][td][color=#03A9F4][b]Potion:[/b][/color][/td][td]"
                    << shortStatDump(b.potionStats, true, true) << " " << incStr("Potion") << "[/td][/tr]" << std::endl;
            if (b.gear.stimulant != EffectSlot::Count)
                oss << "[tr][td][color=#03A9F4][b]Stimulant:[/b][/color][/td][td]"
                    << shortStatDump(s.effectFor(b.gear.stimulant).bonusStats, true, true) << " over 20s "
                    << incStr("Stimulant") << "[/td][/tr]" << std::endl;
            if (!b.gear.kickback.name.empty())
                oss << "[tr][td][color=#03A9F4][b]Kickback:[/b][/color][/td][td]"
                    << shortStatDump(s.effectFor(b.gear.kickback.effect).bonusStats, true, true)
                    << " on [color=#FFEB3B][b]Crit/Pen[/b][/color] over 20s " << incStr("Kickback") << "[/td][/tr]"
                    << std::endl;
            oss << "[/table]" << std::endl;

            oss << "[/color]" << std::endl; // grey end

            auto rot = std::dynamic_pointer_cast<DefaultRotation>(b.rotation);
            oss << std::endl;
            oss << "[size=+1][color=#B2EBF2]Usage[/color][/size]" << std::endl;
            oss << "[list]" << std::endl;
            oss << "[*] Use resources on [color=#03A9F4][b]" << rot->minResourcesForLeftConsumer
                << "[/b][/color] for [color=#03A9F4][b]" << to_string(b.gear.leftWeapon) << "[/b][/color]" << std::endl;
            oss << "[*] Use resources on [color=#03A9F4][b]" << rot->minResourcesForRightConsumer
                << "[/b][/color] for [color=#03A9F4][b]" << to_string(b.gear.rightWeapon) << "[/b][/color]" << std::endl;
            if (rot->tryToConsumeOnBuffed)
            {
                oss << "[*] Always consume when buffed ([color=#03A9F4][b]DABS[/b][/color]";
                if (rot->considerBuffEF)
                    oss << ", [color=#03A9F4][b]Elemental Force[/b][/color]";
                if (rot->considerBuffFF)
                    oss << ", [color=#03A9F4][b]Fatal Flourish[/b][/color]";
                if (rot->considerBuffWC)
                    oss << ", [color=#03A9F4][b]Woodcutters[/b][/color]";
                oss << ")" << std::endl;
            }
            if (rot->buildOnlyBeforeMajorBuffs)
                oss << "[*] Only build resources shortly before DABS" << std::endl;
            if (rot->consumeIfNotBloodOffering)
                oss << "[*] Force blood offering uptime" << std::endl;
            oss << "[/list]" << std::endl;

            if (false) // too much for now
            {
                oss << std::endl;
                oss << "[size=+1][color=#B2EBF2]Damage by Source[/color][/size]" << std::endl;
                oss << "[table][tr][th]Damage[/th][th]Source[/th][th]Crit[/th][th]Pen[/th][/tr]" << std::endl;
                std::vector<std::pair<string, StatLog::DmgStat>> stats;
                for (auto const &kvp : slog.dmgStats)
                    stats.push_back(kvp);
                sort(begin(stats), end(stats),
                     [](std::pair<string, StatLog::DmgStat> const &s1, std::pair<string, StatLog::DmgStat> const &s2) -> bool {
                         return s1.second.totalDmg > s2.second.totalDmg;
                     });
                auto totalDmg = s.totalDmg;
                for (auto const &kvp : stats)
                {
                    auto const &s = kvp.second;
                    oss << "[tr][td]"; // damage
                    oss << "[color=#D32F2F][b][right]" << std::fixed << std::setprecision(2)
                        << s.totalDmg * 100. / totalDmg << "%[/right][/b][/color]";
                    oss << "[/td][td]"; // source
                    oss << "[color=#03A9F4][b]" << kvp.first << "[/b][/color]";
                    oss << "[/td][td]"; // crits
                    oss << std::fixed << std::setprecision(2) << s.crits * 100. / s.hits
                        << "% [color=#B6B6B6]crits[/color]";
                    oss << "[/td][td]"; // pens
                    oss << std::fixed << std::setprecision(2) << s.pens * 100. / s.hits
                        << "% [color=#B6B6B6]pens[/color]";
                    oss << "[/td][/tr]" << std::endl;
                }
                oss << "[/table]" << std::endl;


                oss << std::endl;
                oss << "[size=+1][color=#B2EBF2]Damage by Type[/color][/size]" << std::endl;
                oss << "[table][tr][th]Damage[/th][th]Type[/th][/tr]" << std::endl;
                std::vector<std::pair<double, std::string>> typeDmg = {
                    {-slog.dmgOfType[DmgType::Melee], "Melee"},                             //
                    {-slog.dmgOfType[DmgType::Magic], "Magic"},                             //
                    {-slog.dmgOfType[DmgType::Ranged], "Ranged"},                           //
                    {-slog.dmgOfSkill[SkillType::Builder], "Builder"},                      //
                    {-slog.dmgOfSkill[SkillType::Consumer], "Consumer"},                    //
                    {-slog.dmgOfSub[SubType::Burst], "Burst"},                              //
                    {-slog.dmgOfSub[SubType::Focus], "Focus"},                              //
                    {-slog.dmgOfSub[SubType::Strike], "Strike"},                            //
                    {-slog.dmgOfWeapon[b.gear.leftWeapon], to_string(b.gear.leftWeapon)},   //
                    {-slog.dmgOfWeapon[b.gear.rightWeapon], to_string(b.gear.rightWeapon)}, //
                };
                sort(begin(typeDmg), end(typeDmg));
                for (auto const &kvp : typeDmg)
                {
                    if (kvp.first == 0)
                        continue;

                    oss << "[tr][td]"; // damage
                    oss << "[color=#D32F2F][b][right]" << std::fixed << std::setprecision(2)
                        << -kvp.first * 100. / totalDmg << "%[/right][/b][/color]";
                    oss << "[/td][td]"; // type
                    oss << "[color=#03A9F4][b]" << kvp.second << "[/b][/color]";
                    oss << "[/td][/tr]" << std::endl;
                }
                oss << "[/table]" << std::endl;

                oss << std::endl;
                oss << "[size=+1][color=#B2EBF2]Glyph Stat Impact[/color][/size]" << std::endl;
                oss << "[table][tr][td]Stat[/td]";
                std::vector<int> offsets = {-200, -100, -50, 50, 100, 200};
                std::vector<Rating> ratings = {Rating::Crit, Rating::CritPower, Rating::Pen, Rating::Hit};
                for (auto o : offsets)
                    oss << "[th]" << (o >= 0 ? "+" : "") << o << "[/th]";
                oss << "[/tr]";
                for (auto r : ratings)
                {
                    oss << "[tr][td]"; // rating
                    oss << "[color=#FFEB3B][b]" << to_string(r) << "[/b][/color]";
                    oss << "[/td]"; // offsets
                    for (auto o : offsets)
                    {
                        auto n = "Stat " + to_string(r) + " " + to_string(o);
                        auto d = dmg[n];
                        oss << "[td]";
                        if (d > 1 - eps)
                            oss << "[color=#8BC34A]" << incBaseStr(n) << "[/color]";
                        else
                            oss << "[color=#D32F2F]" << incBaseStr(n) << "[/color]";
                        oss << "[/td]";
                    }
                    oss << "[/tr]" << std::endl;
                }
                oss << "[/table]" << std::endl;
            }
        }

        delete bbs;
    }

    if (parser.isSet(oDumpBuild))
    {
        auto fname = parser.value(oDumpBuild);
        auto json = b.toJson(); // potentially from optimizer
        if (fname == ".")
            std::cout << json.json() << std::endl;
        else
        {
            std::ofstream file(fname.toStdString());
            file << json.json();
            std::cout << "Wrote build to '" << fname.toStdString() << "'" << std::endl;
        }
        std::cout << std::endl;
    }

    if (parser.isSet(oUpdate))
    {
        if (buildIsFile)
        {
            auto json = b.toJson(); // potentially from optimizer
            std::ofstream file(buildFile.toStdString());
            file << json.json();
            std::cout << "Wrote build to '" << buildFile.toStdString() << "'" << std::endl;
        }
        else
        {
            std::cout << "Build was not loaded from a file." << std::endl;
        }
        std::cout << std::endl;
    }

    if (parser.isSet(oDumpScenario))
    {
        auto fname = parser.value(oDumpScenario);
        auto json = s.fightToJson(); // TODO: from optimizer
        json << "Fight Time" << toTimeStr(fightTime);
        json << "Total Time" << toTimeStr(totalTime);
        if (fname == ".")
            std::cout << json.json() << std::endl;
        else
        {
            std::ofstream file(fname.toStdString());
            file << json.json();
            std::cout << "Wrote scenario to '" << fname.toStdString() << "'" << std::endl;
        }
        std::cout << std::endl;
    }

    if (parser.isSet(oTimings))
    {
        aion::ActionAnalyzer::dumpSummary(std::cout, true);
        std::cout << std::endl;
    }
}
