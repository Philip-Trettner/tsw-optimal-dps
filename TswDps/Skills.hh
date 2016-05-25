#pragma once

#include "Skill.hh"

/**
 * @brief Skill library
 *
 * For scalings see https://docs.google.com/spreadsheets/d/1z9b23xHPNQuqmZ5t51SeIMq2rlI6d8mPyWp9BmGNxjc/
 */
struct Skills
{
public:

private:
    template <Weapon weapon, DmgType dmgtype>
    struct Base
    {
    protected:
        Base() = delete;

        static Skill skill(std::string const& name, SkillType skilltype)
        {
            Skill s;
            s.name = name;
            s.weapon = weapon;
            s.dmgtype = dmgtype;
            s.skilltype = skilltype;
            return s;
        }
    };

    static float scaling(std::string const& name);

public:
    struct Pistol : private Base<Weapon::Pistol, DmgType::Ranged>
    {
        static Skill TheBusiness()
        {
            auto s = skill("The Business", SkillType::Builder);
            s.timeIn60th = 60;
            s.hits = 3;
            s.dmgScaling = scaling(s.name);
            return s;
        }

        static Skill HairTrigger()
        {
            auto s = skill("Hair Trigger", SkillType::Builder);
            s.timeIn60th = 60;
            s.hits = 4;
            s.subtype = SubType::Focus;
            s.dmgScaling = scaling(s.name);
            s.channeling = true;
            return s;
        }

        static Skill Shootout()
        {
            auto s = skill("Shootout", SkillType::Consumer);
            s.timeIn60th = 150;
            s.hits = 5;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            s.cooldownIn60th = 60 * 4;
            s.channeling = true;
            return s;
        }
    };

    struct Shotgun : private Base<Weapon::Shotgun, DmgType::Ranged>
    {
        static Skill Striker()
        {
            auto s = skill("Striker", SkillType::Builder);
            s.timeIn60th = 60;
            s.hits = 1;
            s.subtype = SubType::Strike;
            s.dmgScaling = scaling(s.name);
            return s;
        }

        static Skill SingleBarrel()
        {
            auto s = skill("Single Barrel", SkillType::Builder);
            s.timeIn60th = 60;
            s.casttimeIn60th = 30;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            return s;
        }

        static Skill OutForAKill()
        {
            auto s = skill("Out for a Kill", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            s.cooldownIn60th = 60 * 4;
            return s;
        }

        static Skill SureShot()
        {
            auto s = skill("Sure Shot", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            s.cooldownIn60th = 60 * 4;
            return s;
        }

        /// Assumes close range
        static Skill RagingBullet()
        {
            auto s = skill("Raging Bullet", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 1;
            s.subtype = SubType::Strike;
            s.dmgScaling = scaling(s.name + " A @1");
            s.dmgScaling5 = scaling(s.name + " A @5");
            s.cooldownIn60th = 60 * 4;
            return s;
        }

        static Skill PointBlank()
        {
            auto s = skill("Point Blank", SkillType::None);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            s.cooldownIn60th = 60 * 25;

            s.animaDeviation = true;
            s.appliesVulnerability = DmgType::Melee;
            return s;
        }

        static Skill Kneecapper()
        {
            auto s = skill("Kneecapper", SkillType::None);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            s.cooldownIn60th = 60 * 25;

            s.animaDeviation = true;
            return s;
        }

        static Skill Bombardment()
        {
            auto s = skill("Bombardment", SkillType::None);
            s.timeIn60th = 60;
            // ... ???
            s.dmgScaling = scaling(s.name);
            s.cooldownIn60th = 60 * 30;
            return s;
        }

        static Skill LockStockBarrel()
        {
            auto s = skill("Lock, Stock & Barrel", SkillType::None);
            s.cooldownIn60th = 30 * 60;
            s.passive.effect = EffectSlot::LockStockBarrel;
            s.passive.trigger = Trigger::FinishActivation;
            return s;
        }
    };

    struct Rifle : private Base<Weapon::Chaos, DmgType::Ranged>
    {
        static Skill Shellshocker()
        {
            auto s = skill("Shellshocker", SkillType::None);
            s.timeIn60th = 2 * 60;
            s.hits = 8;
            s.dmgScaling = scaling(s.name);
            s.cooldownIn60th = 25 * 60;
            s.channeling = true;

            s.animaDeviation = true;
            s.appliesVulnerability = DmgType::Melee;

            return s;
        }
    };

    struct Chaos : private Base<Weapon::Chaos, DmgType::Magic>
    {
        static Skill RunRampant()
        {
            auto s = skill("Run Rampant", SkillType::Builder);
            s.timeIn60th = 60;
            s.hits = 3;
            s.subtype = SubType::Burst;
            s.dmgScaling = scaling(s.name);
            return s;
        }
        static Skill FourHorsemen()
        {
            auto s = skill("Four Horsemen", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 4;
            s.subtype = SubType::Burst;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            return s;
        }

        static Skill AmorFati()
        {
            auto s = skill("Amor Fati", SkillType::None);
            s.cooldownIn60th = 60 * 60;
            s.passive.trigger = Trigger::FinishActivation;
            s.passive.effect = EffectSlot::AmorFati;
            return s;
        }
    };

    struct Blood : private Base<Weapon::Blood, DmgType::Magic>
    {
    };

    struct Elemental : private Base<Weapon::Elemental, DmgType::Magic>
    {
    };

    struct Blade : private Base<Weapon::Blade, DmgType::Melee>
    {
    };

    struct Hammer : private Base<Weapon::Hammer, DmgType::Melee>
    {
        static Skill Smash()
        {
            auto s = skill("Smash", SkillType::Builder);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            return s;
        }

        static Skill MoltenSteel()
        {
            auto s = skill("Molten Steel", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            s.passive.bonusStats.addedCritChance = 30 / 100.f;
            s.passive.bonusStats.addedCritPower = 15 / 100.f;

            return s;
        }

        static Skill FullMomentum()
        {
            auto s = skill("Full Momentum", SkillType::None);
            s.cooldownIn60th = 30 * 60;
            s.passive.effect = EffectSlot::FullMomentum;
            s.passive.trigger = Trigger::FinishActivation;
            return s;
        }
    };

    struct Fist : private Base<Weapon::Fist, DmgType::Melee>
    {
        static Skill SeeRed()
        {
            auto s = skill("See Red", SkillType::None);
            s.timeIn60th = 4 * 60;
            s.hits = 20;
            s.dmgScaling = scaling(s.name);
            s.cooldownIn60th = 25 * 60;
            s.channeling = true;
            s.animaDeviation = true;
            s.appliesVulnerability = DmgType::Magic;
            return s;
        }

        static Skill Reckless()
        {
            auto s = skill("Reckless", SkillType::None);
            s.cooldownIn60th = 40 * 60;
            s.passive.trigger = Trigger::FinishActivation;
            s.passive.effect = EffectSlot::Reckless;
            return s;
        }
    };

    struct Chainsaw : private Base<Weapon::Aux, DmgType::None>
    {
        static Skill Timber()
        {
            auto s = skill("Timber", SkillType::None);
            s.timeIn60th = 60;
            s.hits = 1;
            s.cooldownIn60th = 15 * 60;
            s.dmgScaling = scaling(s.name);
            s.chanceForScaleInc = 0.33f;
            s.scaleIncPerc = .45f;
            return s;
        }
    };

    static Skill empty() { return Skill(); }
private:
    Skills() = delete;
};

