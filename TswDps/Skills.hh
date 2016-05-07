#pragma once

#include "Skill.hh"

/**
 * @brief Skill library
 *
 * For scalings see https://docs.google.com/spreadsheets/d/1z9b23xHPNQuqmZ5t51SeIMq2rlI6d8mPyWp9BmGNxjc/
 */
struct Skills
{
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

public:
    struct Pistol : private Base<Weapon::Pistol, DmgType::Ranged>
    {
        static Skill TheBusiness()
        {
            auto s = skill("The Business", SkillType::Builder);
            s.timeIn60th = 60;
            s.hits = 3;
            s.dmgScaling = 0.21458f;
            return s;
        }

        static Skill HairTrigger()
        {
            auto s = skill("Hair Trigger", SkillType::Builder);
            s.timeIn60th = 60;
            s.hits = 4;
            s.subtype = SubType::Focus;
            s.dmgScaling = 0.16093f;
            return s;
        }

        static Skill Shootout()
        {
            auto s = skill("Shootout", SkillType::Consumer);
            s.timeIn60th = 150;
            s.hits = 5;
            s.dmgScaling = 0.54241f;
            s.dmgScaling5 = 0.81461f;
            s.cooldownIn60th = 60 * 4;
            return s;
        }

        static Skill DeadlyAim()
        {
            auto s = skill("Deadly Aim", SkillType::None);
            s.timeIn60th = 0;
            // ... ???
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
            s.dmgScaling = 0.64374f;
            return s;
        }

        static Skill SingleBarrel()
        {
            auto s = skill("Single Barrel", SkillType::Builder);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = 0.68050f;
            return s;
        }

        static Skill OutForAKill()
        {
            auto s = skill("Out for a Kill", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = 1.19608f;
            s.dmgScaling5 = 2.36832f;
            s.cooldownIn60th = 60 * 4;
            return s;
        }

        static Skill Bombardment()
        {
            auto s = skill("Bombardment", SkillType::None);
            s.timeIn60th = 60;
            // ... ???
            s.dmgScaling = 1.47226f;
            s.cooldownIn60th = 60 * 30;
            return s;
        }

        static Skill LockStockBarrel()
        {
            auto s = skill("Lock, Stock, Barrel", SkillType::None);
            s.timeIn60th = 0;
            // ... ???
            return s;
        }

        static Skill BreachingShot()
        {
            auto s = skill("Breaching Shot", SkillType::None);
            s.timeIn60th = 0;
            // ... ???
            return s;
        }
    };


private:
    Skills() = delete;
};
