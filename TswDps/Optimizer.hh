#pragma once

#include "common.hh"

#include "Types.hh"
#include "Stats.hh"
#include "Simulation.hh"

#include <set>
#include <random>
#include <unordered_set>

class Optimizer
{
private:
    enum class BuildChange
    {
        Builder,
        NonBuilder,
        EliteActive,
        Passive,
        ElitePassive,
        SignetSwitch,
        SignetChange,
        NeckTalisman,
        StatChange,
        Augment,
        Aux,

        Count
    };

public: // settings
    /// reference simulation (for build and settings)
    Simulation refSim;

    /// testing time overall (per build)
    int timePerTest = 2 * 3600 * 60;
    /// time per individual fight
	int timePerFight = 10 * 60 * 60; // 10 min fights

    /// # active skills
    int maxActives = 7;
    /// # passives
    int maxPassives = 7;
    /// if true, aux weapon is used
    bool useAux = true;

    /// free ratings used for stat optimization
    std::vector<Rating> freeRatings = std::vector<Rating>({Rating::Crit, Rating::CritPower});

public: // tweaks
    /// Number of simulatenously hold builds
    size_t maxActiveBuilds = 30;

    /// Number of new builds per generation
    int newBuildsPerGen = 30;

    /// if true, forces low variance on sim
    bool useLowVariance = true;

    /// exponential bias for better builds
    double topBuildBias = 2.0;

    /// max number of simulateneous build changes
    int maxBuildChanges = 7;

public:
    Optimizer();

    /// Runs the optimization for a number of generations
    void run(int generations = 250);

    /// get the best builds
    std::vector<std::pair<double, Build>> const& getTopBuilds() const { return activeBuilds; }
private: // "Library"
    std::vector<Skill> allSkills;
    std::vector<Skill> allBuilder;
    std::vector<Skill> allNonBuilder;
    std::vector<Skill> allEliteActives;
    std::vector<Skill> allAux;

    std::vector<Passive> allPassives;
    std::vector<Passive> allNonElitePassives;
    std::vector<Passive> allElitePassives;

	std::vector<Augment> allDpsAugments;

	std::vector<Signet> allHeadWeaponSignets;

    std::vector<size_t> headWeaponGearSlots = std::vector<size_t>({ Gear::Head, Gear::WeaponLeft, Gear::WeaponRight });

private: // Transient data
    /// timing
    double secondsSim = -1;

    /// random
    std::default_random_engine random;

    /// top (active) builds (build and dps)
    std::vector<std::pair<double, Build>> activeBuilds;

    /// set of known builds
    std::unordered_set<Build> knownBuilds;

    /// evaluates a given build
    double evaluate(Build const& b);

    /// generates a set of new builds
    /// (may generate already generated builds)
    void generateNewBuilds(int count, std::vector<Build>& builds);

    /// Mutates a given build with a given set of changes
    Build mutateBuild(Build const& build, std::vector<BuildChange> const& changes);

    template <typename T>
    T const& randomElement(std::vector<T> const& elements)
    {
        std::uniform_int_distribution<int> randIdx(0, (int)elements.size() - 1);
        return elements[randIdx(random)];
    }

    /// removes some randomness in build setup
    void normalizeBuild(Build& b);
};
