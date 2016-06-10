#pragma once

#include "Augments.hh"
#include "Passives.hh"
#include "Rotation.hh"
#include "Signets.hh"
#include "Simulation.hh"
#include "Skills.hh"
#include "Skillset.hh"

struct Builds
{
    static Build fromFile(std::string const& filename)
    {
        Build b;
        b.gear.loadEmptyDpsGear(); // for weapons
        b.fromFile(filename);
        return b;
    }

    static Build currTest()
    {
        Build b;
        auto rot = DefaultRotation::create();
        rot->buildOnlyBeforeMajorBuffs = false;
        b.rotation = rot;

        b.skills = {{
                        // skills
                        Skills::Chaos::RunRampant(), //
                        Skills::Elemental::FireManifestation(),   //
                    },
                    {
                        // augs
                        Augments::empty(), //
                        Augments::Quickening(), //
                    },
                    {
                        // passives
                        // Passives::Shotgun::InvasiveMeasures(), //
                    }};

        // gear
        b.gear.loadStandardDpsGear();

        b.gear.pieces[Gear::MajorLeft].signet = Signets::Major::ConeyIslandBand();
        b.gear.pieces[Gear::MinorLeft].signet = Signets::Minor::Equilibrium();

        // hammer ele, i swear, over 9000
        b.gear.leftWeapon = Weapon::Elemental;
        b.gear.rightWeapon = Weapon::Chaos;
        return b;
    }

    static Build random(std::default_random_engine& random)
    {
        std::uniform_real_distribution<float> dice(0.0f, 1.0f);
        auto allPassives = Passives::all();
        std::vector<Weapon> weapons = {Weapon::Blade,     Weapon::Fist,  Weapon::Hammer, Weapon::Blood,  Weapon::Chaos,
                                       Weapon::Elemental, Weapon::Rifle, Weapon::Pistol, Weapon::Shotgun};

        Build b;
        b.gear.loadEmptyDpsGear();

        // weapons
        b.gear.leftWeapon = randomElement(weapons, random);
        do
            b.gear.rightWeapon = randomElement(weapons, random);
        while (b.gear.leftWeapon == b.gear.rightWeapon);

        auto allSkills = filter(Skills::all(), [&](Skill const& s) {
            return s.weapon == b.gear.leftWeapon || s.weapon == b.gear.rightWeapon;
        });
        auto dpsAugs = Augments::allDpsAugs();
        auto minorSigs = Signets::Minor::all();

        // builder
        b.skills.skills[0]
            = randomElement(filter(allSkills, [](Skill const& s) { return s.skilltype == SkillType::Builder; }), random);

        // skills
        int sCnt = std::uniform_int_distribution<int>(0, 6)(random);
        for (auto i = 1; i <= sCnt; ++i)
            b.skills.skills[i] = randomElement(allSkills, random);

        // augs
        for (auto i = 0; i <= sCnt; ++i)
            if (b.skills.skills[i].slotForDmgAug && dice(random) < .5)
                b.skills.augments[i] = randomElement(dpsAugs, random); // TODO: uniqueness?

        // passives
        int pCnt = std::uniform_int_distribution<int>(0, 7)(random);
        for (auto i = 0; i < pCnt; ++i)
            b.skills.passives.push_back(randomElement(allPassives, random));

        // rot
        auto rot = DefaultRotation::create();
        rot->buildOnlyBeforeMajorBuffs = dice(random) < .5;
        rot->considerBuffEF = dice(random) < .5;
        rot->considerBuffFF = dice(random) < .5;
        rot->considerBuffWC = dice(random) < .5;
        rot->consumeIfNotBloodOffering = dice(random) < .5;
        rot->tryToConsumeOnBuffed = dice(random) < .5;
        rot->minResourcesForLeftConsumer = std::uniform_int_distribution<int>(1, 5)(random);
        rot->minResourcesForRightConsumer = std::uniform_int_distribution<int>(1, 5)(random);
        b.rotation = rot;

        // gear
        std::vector<Rating> ratings = {Rating::Hit, Rating::Pen, Rating::Crit, Rating::CritPower};
        int si = 0;
        for (auto& p : b.gear.pieces)
        {
            p.free(randomElement(ratings, random));

            if (p.slot == Gear::Slot::Minor)
                p.signet = randomElement(minorSigs, random);

            if (si == Gear::MinorLeft && dice(random) < .5)
                p.signet = Signets::Minor::SubwayTokens();

            if (si == Gear::MajorMid && dice(random) < .5)
                p.signet = Signets::Major::WoodcuttersWrath();
            if (si == Gear::MajorMid && dice(random) < .33)
                p.signet = Signets::Major::EgonPendant();

            ++si;
        }

        return b;
    }

private:
    Builds() = delete;
};
