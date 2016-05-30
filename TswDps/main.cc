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
#include "SkillTable.hh"
#include "Optimizer.hh"

int main(int argc, char *argv[])
{
    SkillTable::loadSkillTable(pathOf(__FILE__) + "/Skill Scaling - Scalings.tsv");

    (void)argc;
    (void)argv;

    const bool optimization = true;

    const bool dpsTest = false;
	
	const int maxTime = 100 * 1000 * 60;
    const int burstFight = 13 * 60;
    const bool longRun = true;
    const bool buffs = true;

    // for (auto const& s : g.enumerateGearStats({Rating::Crit,
    // Rating::CritPower}, true))
    //    std::cout << s.critRating << ";" << s.critPowerRating << std::endl;

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
	//s.loadBuild(Builds::currMaxFistHammer());
	// s.loadBuild(Builds::currMaxPistolShotgun());
	//s.loadBuild(Builds::procHairtriggerOnly());
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

        auto const& builds = optimizer.getTopBuilds();
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
