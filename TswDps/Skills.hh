#include "Skill.hh"

struct Skills
{
private:
    template<Weapon weapon, DmgType dmgtype>
    struct Base {
    protected:
        Base() = delete;

        static Skill skill(std::string const& name, SkillType skilltype) {
            Skill s;
            s.name = name;
            s.weapon = weapon;
            s.dmgtype = dmgtype;
            s.skilltype = skilltype;
            return s;
        }
    };

public:
    struct Pistol : private Base<Weapon::Pistol, DmgType::Ranged> {
        static Skill TheBusiness() {
            auto s = skill("The Business", SkillType::Builder);
            s.timeIn60th = 60;
            s.hits = 3;
            s.dmgScaling = -1; // TODO
            return s;
        }

        static Skill HairTrigger() {
            auto s = skill("Hair Trigger", SkillType::Builder);
            s.timeIn60th = 60;
            s.hits = 4;
            s.subtype = SubType::Focus;
            s.dmgScaling = -1; // TODO
            return s;
        }

        static Skill Shootout() {
            auto s = skill("Shootout", SkillType::Consumer);
            s.timeIn60th = 150;
            s.hits = 5;
            s.dmgScaling = -1; // TODO
            return s;
        }

        static Skill DeadlyAim() {
            auto s = skill("Deadly Aim", SkillType::None);
            s.timeIn60th = 0;
            // ... ???
            return s;
        }
    };

    struct Shotgun : private Base<Weapon::Shotgun, DmgType::Ranged> {
        static Skill Striker() {
            auto s = skill("Striker", SkillType::Builder);
            s.timeIn60th = 60;
            s.hits = 1;
            s.subtype = SubType::Strike;
            s.dmgScaling = -1; // TODO
            return s;
        }

        static Skill SingleBarrel() {
            auto s = skill("Single Barrel", SkillType::Builder);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = -1; // TODO
            return s;
        }

        static Skill OutForAKill() {
            auto s = skill("Out for a Kill", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = -1; // TODO
            return s;
        }

        static Skill Bombardment() {
            auto s = skill("Bombardment", SkillType::None);
            s.timeIn60th = 60;
            // ... ???
            return s;
        }

        static Skill LockStockBarrel() {
            auto s = skill("Lock, Stock, Barrel", SkillType::None);
            s.timeIn60th = 0;
            // ... ???
            return s;
        }

        static Skill BreachingShot() {
            auto s = skill("Breaching Shot", SkillType::None);
            s.timeIn60th = 0;
            // ... ???
            return s;
        }
    };


private:
    Skills() = delete;
};
