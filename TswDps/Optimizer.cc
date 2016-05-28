#include "Optimizer.hh"

#include "Skills.hh"
#include "Augments.hh"
#include "Passives.hh"
#include "Signets.hh"

#include <algorithm>

#include <chrono>

#include <thread>
#include <mutex>

Optimizer::Optimizer()
{
}

void Optimizer::run(int generations)
{
    // seed
    auto startBuild = refSim.build();
    normalizeBuild(startBuild);
    knownBuilds.insert(startBuild);
    activeBuilds.push_back(std::make_pair(evaluate(startBuild), startBuild));

    // init lib
    allSkills = Skills::all();
    allBuilder.clear();
    allEliteActives.clear();
    allNonBuilder.clear();
    allAux.clear();
    for (auto const& s : allSkills)
    {
        if (s.weapon == Weapon::Aux)
            allAux.push_back(s);

        if (s.weapon != startBuild.gear.leftWeapon && s.weapon != startBuild.gear.rightWeapon)
            continue; // non-matched weapon

        if (s.skilltype == SkillType::Builder)
            allBuilder.push_back(s);
        else if (s.skilltype == SkillType::Elite)
            allEliteActives.push_back(s);
        else
            allNonBuilder.push_back(s);
    }

    allPassives = Passives::all();
    allNonElitePassives.clear();
    allElitePassives.clear();
    for (auto const& p : allPassives)
    {
        assert(p.passivetype != PassiveType::Skill);

        if (p.passivetype == PassiveType::Elite)
            allElitePassives.push_back(p);
        else
            allNonElitePassives.push_back(p);
    }

    allDpsAugments = Augments::allDpsAugs();

	allHeadWeaponSignets = Signets::HeadWeapon::all();

    // generations
    for (auto g = 0; g < generations; ++g)
    {
        std::cout << "GENERATION " << g << std::endl;

        secondsSim = 0;
        auto now = std::chrono::system_clock::now();

        std::vector<Build> newBuilds;
        generateNewBuilds(newBuildsPerGen, newBuilds);

		// preprocess
		for (auto i = (int)newBuilds.size() - 1; i >= 0; --i)
		{
			auto const& b = newBuilds[i];
			if (knownBuilds.count(b))
				newBuilds.erase(begin(newBuilds) + i);
			else knownBuilds.insert(b);
		}
		std::cout << "  - testing " << newBuilds.size() << " new builds" << std::endl;

		// MT
		auto nowEval = std::chrono::system_clock::now();
		std::mutex m;
		std::vector<std::thread> threads;
		auto threadFunc = [&]()
		{
			while (true) 
			{
				// get a build
				m.lock();
				if (newBuilds.empty()) // finished?
				{
					m.unlock();
					return;
				}
				auto b = newBuilds[newBuilds.size() - 1];
				newBuilds.erase(newBuilds.begin() + (newBuilds.size() - 1));
				m.unlock();

				// evaluate
				auto res = std::make_pair(evaluate(b), b);

				// report results
				m.lock();
				activeBuilds.push_back(res);
				m.unlock();
			}
		};

		// start threads
		for (auto i = 0; i < 8; ++i)
			threads.push_back(std::thread(threadFunc));

		// stop threads
		for (auto i = 0; i < 8; ++i)
			threads[i].join();
		secondsSim += std::chrono::duration<double>(std::chrono::system_clock::now() - nowEval).count();

        // sort builds by dps
        sort(begin(activeBuilds), end(activeBuilds), [](std::pair<double, Build> const& l, std::pair<double, Build> const& r)
             {
                 return l.first > r.first;
             });
        // ascending
        assert(activeBuilds.size() < 2 || activeBuilds[0].first > activeBuilds[1].first);

        // truncate if too many active builds
        if (activeBuilds.size() > maxActiveBuilds)
            activeBuilds.erase(activeBuilds.begin() + maxActiveBuilds, activeBuilds.end());

        // timing
        auto totalTime = std::chrono::duration<double>(std::chrono::system_clock::now() - now).count();
        auto nonSimTime = totalTime - secondsSim;
        std::cout << "  - highest DPS: " << activeBuilds[0].first << std::endl;
        std::cout << "  - simulation: " << secondsSim << " seconds" << std::endl;
        std::cout << "  - other:      " << nonSimTime << " seconds" << std::endl;
    }
}

double Optimizer::evaluate(const Build& b)
{
    auto sim = refSim;
    sim.loadBuild(b);
    if (useLowVariance)
        sim.lowVarianceMode = true;
    sim.log = nullptr;
    sim.init();

    auto totalTime = 0;
    while (totalTime < timePerTest)
    {
        totalTime += timePerFight;
        sim.simulate(timePerFight);
    }

    return sim.totalDPS();
}

void Optimizer::generateNewBuilds(int count, std::vector<Build>& builds)
{
    std::uniform_int_distribution<int> randomBuildIdx(0, (int)activeBuilds.size() - 1);
    std::uniform_int_distribution<int> randomBuildChanges(1, maxBuildChanges);
    std::uniform_int_distribution<int> randomChange(0, (int)BuildChange::Count - 1);
    std::uniform_real_distribution<float> dice(0.0f, 1.0f);

    auto maxDps = activeBuilds[0].first;

    for (auto b = 0; b < count; ++b)
    {
        // get random base build
        // distributed according to dps
        auto idx = randomBuildIdx(random);
        while (true)
        {
            if (dice(random) < pow(activeBuilds[idx].first / maxDps, topBuildBias))
                break;
            idx = randomBuildIdx(random);
        }
        auto const& base = activeBuilds[idx].second;

        // generate build changes
        auto nrChanges = randomBuildChanges(random);
        std::vector<BuildChange> changes;
        while (nrChanges > 0)
        {
            changes.push_back((BuildChange)randomChange(random));
            --nrChanges;
        }

        // add new, mutated build
        builds.push_back(mutateBuild(base, changes));
    }
}

Build Optimizer::mutateBuild(const Build& build, const std::vector<Optimizer::BuildChange>& changes)
{
    std::uniform_real_distribution<float> dice(0.0f, 1.0f);
    std::uniform_int_distribution<int> randomSkill(0, maxActives - 1);
	std::uniform_int_distribution<int> randomPassive(0, maxPassives - 1);
	std::uniform_int_distribution<int> randomGearSlot(Gear::Head, Gear::WeaponRight);
	std::uniform_int_distribution<int> randomNeck(0, 2);

    auto b = build;

    auto resortPassives = false;

    auto changed = false;

    for (auto const& c : changes)
    {
        switch (c)
        {
        case BuildChange::Builder:
            for (auto& s : b.skills.skills)
                if (s.skilltype == SkillType::Builder)
                {
                    s = randomElement(allBuilder);
                    changed = true;
                    break;
                }
            assert(changed); // assume only one builder
            break;
        case BuildChange::NonBuilder:
            // replace random non-builder non-elite
            while (true)
            {
                auto idx = randomSkill(random);
                if (b.skills.skills[idx].skilltype != SkillType::Builder && //
                    b.skills.skills[idx].skilltype != SkillType::Elite)
                {
                    auto newSkill = randomElement(allNonBuilder);
                    auto valid = true;
                    for (auto const& s : b.skills.skills)
                        if (s.name == newSkill.name)
                        {
                            valid = false;
                            break;
                        }
                    if (valid) // no duplicates
                    {
                        b.skills.skills[idx] = newSkill;

                        // ensure augment can be slotted
                        if (!b.skills.skills[idx].slotForDmgAug && b.skills.augments[idx].slot == AugmentSlot::Damage)
                            b.skills.augments[idx] = Augments::empty();
                    }
                    changed = true;
                    break;
                }
            }
            break;
        case BuildChange::EliteActive:
            // dmg augments don't need adjustment, cause active elites always have them
            for (auto& s : b.skills.skills)
                if (s.skilltype == SkillType::Elite)
                {
                    if (dice(random) < 0.1)
                        s = Skills::empty(); // 10% chance to unslot elite
                    else
                        s = randomElement(allEliteActives);
                    changed = true;
                    break;
                }
            if (!changed) // no elite slotted currently
            {
                // replace random non-builder
                while (true)
                {
                    auto idx = randomSkill(random);
                    if (b.skills.skills[idx].skilltype != SkillType::Builder)
                    {
                        b.skills.skills[idx] = randomElement(allEliteActives);
                        changed = true;
                        break;
                    }
                }
            }
            break;
        case BuildChange::Passive:
            // replace random non-elite
            while (true)
            {
                auto idx = randomPassive(random);
                if (b.skills.passives[idx].passivetype != PassiveType::Elite)
                {
                    auto newPassive = randomElement(allNonElitePassives);
                    auto valid = true;
                    for (auto const& s : b.skills.passives)
                        if (s.name == newPassive.name)
                        {
                            valid = false;
                            break;
                        }
                    if (valid) // no duplicates
                        b.skills.passives[idx] = newPassive;
                    changed = true;
                    resortPassives = true;
                    break;
                }
            }
            break;
        case BuildChange::ElitePassive:
            for (auto& p : b.skills.passives)
                if (p.passivetype == PassiveType::Elite)
                {
                    if (dice(random) < 0.1)
                        p = Passives::empty(); // 10% chance to unslot elite
                    else
                        p = randomElement(allElitePassives);
                    changed = true;
                    resortPassives = true;
                    break;
                }
            if (!changed) // no elite slotted currently
            {
                // replace random non-builder
                while (true)
                {
                    auto idx = randomPassive(random);
                    b.skills.passives[idx] = randomElement(allElitePassives);
                    changed = true;
                    resortPassives = true;
                    break;
                }
            }
            break;
        case BuildChange::SignetChange:
            while (!changed)
            {
				auto idx = randomElement(headWeaponGearSlots);

				b.gear.pieces[idx].signet = randomElement(allHeadWeaponSignets);
				changed = true;
            }
            break;
        case BuildChange::SignetSwitch:
			while (!changed)
			{
				auto idx1 = randomElement(headWeaponGearSlots);
				auto idx2 = randomElement(headWeaponGearSlots);

				if (idx1 == idx2)
					continue;

				auto tmp = b.gear.pieces[idx1].signet;
				b.gear.pieces[idx1].signet = b.gear.pieces[idx2].signet;
				b.gear.pieces[idx2].signet = tmp;
				changed = true;
			}
            break;
        case BuildChange::NeckTalisman:
			while (!changed)
			{
				auto type = randomNeck(random);

				// TODO!
				switch (type)
				{
				case 0: // normal
					break;
				case 1: // woodcutters
					break;
				case 2: // egon
					break;
				}

				changed = true;
			}
            break;
        case BuildChange::StatChange:
			while (!changed)
			{
				auto idx = randomGearSlot(random);
				if (b.gear.pieces[idx].status != Gear::SlotStatus::Free)
					continue;

				assert(!freeRatings.empty());
				if (freeRatings.size() == 1 || dice(random) < .5) // split or pure
				{
					b.gear.pieces[idx].free(randomElement(freeRatings));
				}
				else
				{
					Rating r1, r2;
					do
					{
						r1 = randomElement(freeRatings);
						r2 = randomElement(freeRatings);
					} while (r1 == r2);

					b.gear.pieces[idx].free(r1);
					b.gear.pieces[idx].free(r2);
					changed = true;
				}
			}
            break;
        case BuildChange::Augment:
            while (!changed)
            {
                // choose non-empty dmg skill
                auto idx = randomSkill(random);
                if (b.skills.skills[idx].name.empty())
                    continue;
                if (!b.skills.skills[idx].slotForDmgAug)
                    continue;

                // choose new aug
                auto newAug = randomElement(allDpsAugments);
                while (b.skills.augments[idx].name == newAug.name)
                    newAug = randomElement(allDpsAugments);

                // switch if required
                for (auto i = 0; i < maxActives; ++i)
                    if (b.skills.augments[i].name == newAug.name)
                    {
                        b.skills.augments[i] = b.skills.augments[idx];
                        break;
                    }

                // assign new
                b.skills.augments[idx] = newAug;
                changed = true;
            }
            break;
        case BuildChange::Aux:
            // TODO
            break;

        case BuildChange::Count:
            assert(false);
            break;
        }
    }

    if (resortPassives)
        normalizeBuild(b);

    return b;
}

void Optimizer::normalizeBuild(Build& b)
{
	while (b.skills.passives.size() < maxPassives)
		b.skills.passives.push_back(Passives::empty());
	sort(begin(b.skills.passives), end(b.skills.passives), [](Passive const& l, Passive const& r)
         {
             return l.name < r.name;
         });
}
