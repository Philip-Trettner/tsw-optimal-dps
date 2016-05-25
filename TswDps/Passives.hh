#pragma once

#include "Effects.hh"
#include "Passive.hh"

#include "SkillTable.hh"

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
    static float scaling(std::string const& name) { return SkillTable::scaling(name); }
public:
    struct Pistol : private Base<Weapon::Pistol, DmgType::Ranged>
    {
        static Passive SealTheDeal()
        {
            auto p = passive("Seal The Deal", PassiveType::None);

            p.restrictType = SkillType::Consumer;
            p.bonusStats.addedCritChance = 5 / 100.f;
            p.bonusStats.additiveDamage = 20 / 100.f;

            return p;
        }

        static Passive DoubleUp()
        {
            auto p = passive("Double Up", PassiveType::None);

            p.restrictWeapon = true;
            p.trigger = Trigger::Hit;
            p.triggerChance = 0.15f;
            p.effect = EffectSlot::DoubleUp;

            return p;
        }

        static Passive OneInTheChamber()
        {
            auto p = passive("One In The Chamber", PassiveType::None);

            p.trigger = Trigger::Crit;
            p.effect = EffectSlot::OneInTheChamber;

            return p;
        }
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
        static Passive Lethality()
        {
            auto p = passive("Lethality", PassiveType::None);

            p.trigger = Trigger::Hit;
            p.effect = EffectSlot::Lethality;

            return p;
        }
    };

    struct Blade : private Base<Weapon::Blade, DmgType::Melee>
    {
        static Passive TwistTheKnife()
        {
            auto p = passive("Twist The Knife", PassiveType::None);

            p.trigger = Trigger::Pen;
            p.effect = EffectSlot::TwistTheKnife;

            return p;
        }

        static Passive SuddenReturn()
        {
            auto p = passive("Sudden Return", PassiveType::None);

            p.trigger = Trigger::Pen;
            p.effect = EffectSlot::SuddenReturn;

            return p;
        }

        static Passive FortunateStrike()
        {
            auto p = passive("Fortunate Strike", PassiveType::None);

            p.trigger = Trigger::Hit;
            p.triggerChance = 0.10f;
            p.effect = EffectSlot::FortunateStrike;

            return p;
        }
    };

    struct Hammer : private Base<Weapon::Hammer, DmgType::Melee>
    {
        static Passive Thunderstruck()
        {
            auto p = passive("Thunderstruck", PassiveType::None);

            p.trigger = Trigger::Crit;
            p.effect = EffectSlot::Thunderstruck;

            return p;
        }

        static Passive Brawler()
        {
            auto p = passive("Brawler", PassiveType::None);

            p.bonusStats.addedCritPower = .15f;

            return p;
        }

        static Passive Momentum()
        {
            auto p = passive("Momentum", PassiveType::None);

            p.trigger = Trigger::Hit;
            p.effect = EffectSlot::MomentumStack;
            p.restrictWeapon = true;

            return p;
        }
    };

    struct Fist : private Base<Weapon::Fist, DmgType::Melee>
    {
    };

    struct Blood : private Base<Weapon::Blood, DmgType::Magic>
    {
        static Passive IronMaiden()
        {
            auto p = passive("Iron Maiden", PassiveType::None);

            p.trigger = Trigger::Pen;
            p.effect = EffectSlot::MinorPenetrationChance;

            return p;
        }
    };

    struct Chaos : private Base<Weapon::Chaos, DmgType::Magic>
    {
        static Passive Calamity()
        {
            auto p = passive("Calamity", PassiveType::None);

            p.restrictWeapon = true;
            p.trigger = Trigger::Hit;
            p.triggerChance = 0.15f;
            p.effect = EffectSlot::Calamity;

            return p;
        }

        static Passive Gnosis()
        {
            auto p = passive("Gnosis", PassiveType::None);

            p.trigger = Trigger::Hit;
            p.triggerChance = 0.333f;
            p.effect = EffectSlot::Gnosis;

            return p;
        }
    };

    struct Elemental : private Base<Weapon::Elemental, DmgType::Magic>
    {
        static Passive ElementalForce()
        {
            auto p = passive("Elemental Force", PassiveType::None);

            p.trigger = Trigger::FinishActivation;
            p.effect = EffectSlot::ElementalForceStacks;

            return p;
        }

        static Passive ElementalOverload()
        {
            auto p = passive("Elemental Overload", PassiveType::None);

            p.restrictWeapon = true;
            p.trigger = Trigger::Hit;
            p.triggerChance = 0.333f;
            p.effect = EffectSlot::ElementalOverload;

            return p;
        }
    };

    static Passive empty() { return Passive(); }
private:
    Passives() = delete;
};
