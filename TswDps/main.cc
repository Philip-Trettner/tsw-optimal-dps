#include <iostream>
#include <algorithm>
#include <fstream>
#include <chrono>

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

#define CLI 0

enum class ExploreType
{
    Best,
    Burst,
    Dummy,
};

void explore(ExploreType type, double timeMult);

#if CLI
int main(int argc, char *argv[])
{
	SkillTable::loadSkillTable("Skill Scaling - Scalings.tsv");

	if (argc != 2)
	{
		std::cout << "Usage:" << std::endl;
		std::cout << "  detailed fight (20s fight): -f BUILDFILE" << std::endl;
		std::cout << "  dps analysis   (10h fight): -s BUILDFILE" << std::endl;
		std::cout << "  optimize        (100 gens): -o BUILDFILE" << std::endl;
		return 0;
	}

	auto flag = argv[0];
	auto file = argv[1];

	if (flag == "-f")
	{
		
	}
	else if (flag == "-s")
	{
		
	}
	else if (flag == "-o")
	{
		
	}
	else
	{
		std::cout << "invalid flag " << flag << std::endl;
	}
}
#endif

#if CLI
int main_(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    SkillTable::loadSkillTable(pathOf(__FILE__) + "/Skill Scaling - Scalings.tsv");
	
    (void)argc;
    (void)argv;

    /**
     * TODO:
     *
     * * Mercurials
     * * Coney, Equilibrium
     *
     * later: afflictions + signet of corruption
     *
     * Doom is strange... is consumed on focus/burst builders?
     */

    Optimizer optimizer;

	const bool continuousExploration = true;
	const double longerTimePerRun = 1.1;
    const auto exploreType = ExploreType::Best;
    const bool exploration = !true;
    const bool optimization = !true;

    const bool dpsTest = true;
	const bool varianceComparison = !true;

    const int maxTime = 100 * 1000 * 60;
    const int burstFight = 20 * 60;
    const bool longRun = true;
    const bool buffs = true;

    optimizer.excludeSkillsAndPassives = {
        //"Power Line",         //
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

    s.loadBuild(Builds::fromFile(pathOf(__FILE__) + "/results/best/Elemental-Hammer.json"));
    //s.loadBuild(Builds::fromFile(pathOf(__FILE__) + "/results/best/Elemental-Blood.json"));
    //s.loadBuild(Builds::currTest());
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
        s.analyzePassiveContribution(maxTime);
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
			o.timePerTest = (int)(o.timePerTest * timeMult);
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
				s.enemyInfo.baseVulnerability = 0;      // no expose
				s.enemyInfo.penPower = 0.49f;           // no breaching
                o.timePerFight = 2 * 60 * 60;           // 2 min parses
                s.buffAt = INF_TIME;                    // no buffs
				s.enemyInfo.stats.blockRating = 100;
				s.enemyInfo.stats.defenceRating = 100;
                break;
            }

            std::cout << "TESTING " << to_string(w1) << " and " << to_string(w2) << (cont ? " [cont.]" : "") << std::flush;
            o.run(15);

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
