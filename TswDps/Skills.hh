#include "Skill.hh"

struct Skills
{
    static Skill Striker() { return {"Striker", Weapon::Shotgun, 1.0f, 1, 625, SubType::Strike}; }
    static Skill TheBusiness() { return {"The Business", Weapon::Pistol, 1.0f, 3, 208 * 3, SubType::None}; }
    static Skill HairTrigger() { return {"Hair Trigger", Weapon::Pistol, 1.0f, 4, 156 * 4, SubType::Focus}; }
    static Skill SingleBarrel() { return {"Single Barrel", Weapon::Shotgun, 1.0f, 1, 660, SubType::None}; }
    static Skill OutForAKill(int res)
    {
        return {"Out for a Kill", Weapon::Shotgun, 1.0f, 1, 1160.f + (2297 - 1160) * res / 5, SubType::None};
    }
    static Skill Shootout() // res?
    {
        return {"Shootout", Weapon::Pistol, 2.5f, 5, 3950, SubType::Focus};
    }

private:
    Skills() = delete;
};
