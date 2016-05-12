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
            p.weaponType = weapon;
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
        static Passive DeadOnTarget()
        {
            auto p = passive("Dead On Target", PassiveType::None);

            p.restrictWeapon = true;
            p.bonusStats.additiveDamage = .10f;

            return p;
        }
    };

    struct Rifle : private Base<Weapon::Rifle, DmgType::Ranged>
    {
    };

    struct Blade : private Base<Weapon::Blade, DmgType::Melee>
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

            p.bonusStats.addedCritPower = .15f;

            return p;
        }
    };

    struct Fist : private Base<Weapon::Fist, DmgType::Melee>
    {
    };

    struct Blood : private Base<Weapon::Blood, DmgType::Magic>
    {
    };

    struct Chaos : private Base<Weapon::Chaos, DmgType::Magic>
    {
    };

    struct Elemental : private Base<Weapon::Elemental, DmgType::Magic>
    {
    };

private:
    Passives() = delete;
};
