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
        SkillSwitch,
        NonBuilder,
        EliteActive,
        Passive,
        ElitePassive,
        MajorSignetSwitch,
        MajorSignetChange,
        MinorSignetChange,
        NeckTalisman,
        FingerTalisman,
        StatChange,
        Augment,
		Aux,
		Rotation,
        Potion,
        Stimulant,
        Kickback,

        Count
    };

public: // settings
    /// reference simulation (for build and settings)
    Simulation refSim;

    /// testing time overall (per build)
    int timePerTest = 1 * 3600 * 60;
    /// time per individual fight
    int timePerFight = 1.5 * 60 * 60; // 1.5 min fights

    /// # active skills
    int maxActives = 7;
    /// # passives
    int maxPassives = 7;
	/// if true, aux weapon is used TODO
	bool useAux = true;
	/// elites
	bool useEliteActive = true;
	bool useElitePassive = true;
	// if true, empty signets can be changed
	bool switchEmptySignets = true;
	// if true, builder is not changed
	bool fixedBuilder = false;

    // if true, allows for woodcutters usage
    bool allowWoodcutters = true;

    // if true, also allows split ql11
    bool allowSplitGlyphs = true;

    /// free ratings used for stat optimization
    std::vector<Rating> freeRatings = std::vector<Rating>({Rating::Crit, Rating::CritPower, Rating::Pen, Rating::Hit});

    /// excluded skills and passives
    std::set<std::string> excludeSkillsAndPassives;

    /// default talisman quality
    Gear::TalismanQuality defaultQuality = Gear::TalismanQuality::QL11;
    /// raid item quality (TODO: implement .4 version)
    Gear::TalismanQuality raidQuality = Gear::TalismanQuality::QL10_9;

    /// force a certain vulnerability (if none found, returns immediately, leaving zero top builds)
    DmgType forceVulnerability = DmgType::None;

public: // tweaks
    /// Number of simulatenously hold builds
    size_t maxActiveBuilds = 50;

    /// Number of new builds per generation
    int newBuildsPerGen = 60;

    /// if true, forces low variance on sim
    bool useLowVariance = true;

    /// exponential bias for better builds
    double topBuildBias = 2.0;

    /// max number of simulateneous build changes
    int maxBuildChanges = 7;

    /// if true, minimizes output
    bool silent = false;

    /// if true, uses multithreading
    bool multithreaded = true;

    /// if > 0, nr of threads
    int threadOverwrite = 0;

public: // stats
	/// nr of builds evaluated
	int totalBuildsEvaluated = 0;

public:
    Optimizer();

    /// Runs the optimization for a number of generations
    void run(int generations = 50);

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
    std::vector<Augment> allSupportAugments;

	std::vector<Signet> allHeadWeaponSignets;
    std::vector<Signet> allMinorSignets;

    std::vector<size_t> headWeaponGearSlots = std::vector<size_t>({ Gear::Head, Gear::WeaponLeft, Gear::WeaponRight });
    std::vector<size_t> minorGearSlots = std::vector<size_t>({ Gear::MinorLeft, Gear::MinorRight, Gear::MinorMid });

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

    /// removes some randomness in build setup
    void normalizeBuild(Build& b);

    template <typename T>
    T const& randomElement(std::vector<T> const& elements)
    {
        std::uniform_int_distribution<int> randIdx(0, (int)elements.size() - 1);
        return elements[randIdx(random)];
    }
};
