#pragma once

#include "Passive.hh"

/**
 * @brief Passive library
 *
 * For scalings see https://docs.google.com/spreadsheets/d/1z9b23xHPNQuqmZ5t51SeIMq2rlI6d8mPyWp9BmGNxjc/
 */
struct Passives
{
private:
    template <Weapon weapon, DmgType dmgtype>
    struct Base
    {
    protected:
        Base() = delete;

        static Passive passive(std::string const& name, PassiveType passivetype)
        {
            Passive p;
            p.name = name;
            p.weapon = weapon;
            p.dmgtype = dmgtype;
            p.passivetype = passivetype;
            return p;
        }
    };

public:
    struct Pistol : private Base<Weapon::Pistol, DmgType::Ranged>
    {
    };

    struct Shotgun : private Base<Weapon::Shotgun, DmgType::Ranged>
    {

    };

    struct Hammer : private Base<Weapon::Hammer, DmgType::Melee>
    {
        static Passive Thunderstruck()
        {
            auto p = passive("Thunderstruck", PassiveType::None);


            return p;
        }

        static Passive Brawler()
        {
            auto p = passive("Brawler", PassiveType::None);


            return p;
        }
    };


private:
    Passives() = delete;
};
