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
            auto p = passive("Seal The Deal", PassiveType::Elite);

            p.restrictType = SkillType::Consumer;
            p.bonusStats.addedCritChance = 5 / 100.f;
            p.bonusStats.addedPenChance = 5 / 100.f;
            p.bonusStats.additiveDamage = 20 / 100.f;

            return p;
        }

        static Passive DoubleUp()
        {
            auto p = passive("Double Up", PassiveType::Skill);

            p.restrictWeapon = true;
            p.trigger = Trigger::Hit;
            p.triggerChance = 0.15f * 0; // BUG: Not logged, not in ACT, uncool :(
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

        static Passive Closer()
        {
            auto p = passive("Closer", PassiveType::None);

            p.restrictType = SkillType::Consumer;
            p.bonusStats.additiveDamage = 10 / 100.f;

            return p;
        }

        static Passive StraightShooter()
        {
            auto p = passive("Straight Shooter", PassiveType::None);

            p.restrictWeapon = true;
            p.bonusStats.additiveDamage = 10 / 100.f;

            return p;
        }

        static Passive QuickFortyFive()
        {
            auto p = passive("Quick Forty Five", PassiveType::None);

            // TODO: quick forty five -0.5s

            return p;
        }

        static Passive IncreasedFocus()
        {
            auto p = passive("Increased Focus", PassiveType::None);

            p.restrictSubType = SubType::Focus;
            p.bonusStats.addedCritChance = 7.5f / 100.f;

            return p;
        }

        static Passive MadSkills()
        {
            auto p = passive("Mad Skills", PassiveType::None);

            p.bonusStats.critRating = 200; // better than stacks

            return p;
        }

        static Passive RapidGetaway()
        {
            auto p = passive("Rapid Getaway", PassiveType::Elite);

            p.restrictType = SkillType::Consumer;
            p.trigger = Trigger::FinishActivation;
            p.effect = EffectSlot::MinorCriticalChance;

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

        static Passive BodyPiercing()
        {
            auto p = passive("Body Piercing", PassiveType::None);

            p.bonusStats.penRating = 200; // better than stacks

            return p;
        }

        static Passive StrikeForce()
        {
            auto p = passive("Strike Force", PassiveType::None);

            p.restrictSubType = SubType::Strike;
            p.bonusStats.addedPenChance = 7.5f / 100.f;

            return p;
        }

        static Passive RapidReload()
        {
            auto p = passive("Rapid Reload", PassiveType::None);

            // TODO: Buckshot -0.5s

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

        static Passive ImprovedBursts()
        {
            auto p = passive("Improved Bursts", PassiveType::None);

            p.restrictSubType = SubType::Burst;
            p.bonusStats.additiveDamage = 10 / 100.f;

            return p;
        }

        static Passive Accuracy()
        {
            auto p = passive("Accuracy", PassiveType::None);

            p.bonusStats.hitRating = 75;
            p.bonusStats.additiveDamage = 7 / 100.f;

            return p;
        }

        static Passive CallYourShots()
        {
            auto p = passive("Call Your Shots", PassiveType::None);

            p.bonusStats.hitRating = 200;

            return p;
        }

        static Passive ShortControlledBursts()
        {
            auto p = passive("Short Controlled Bursts", PassiveType::None);

            p.restrictSubType = SubType::Burst;
            p.bonusStats.hitRating = 200;
            p.bonusStats.addedCritChance = 4 / 100.f;
            p.bonusStats.addedPenChance = 4 / 100.f;

            return p;
        }

        static Passive EagleEye()
        {
            auto p = passive("Eagle Eye", PassiveType::None);

            p.restrictWeapon = true;
            p.bonusStats.additiveDamage = .10f;

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

        static Passive FatalFlourish()
        {
            auto p = passive("Fatal Flourish", PassiveType::None);

            // TODO: affliction!
            p.trigger = Trigger::FinishActivation;
            p.triggerOnDamaging = true;
            p.effect = EffectSlot::FatalFlourishStacks;

            return p;
        }

        static Passive SevenAndAHalfSamurai()
        {
            auto p = passive("Seven and a half Samurai", PassiveType::None);

            p.restrictSubType = SubType::Focus;
            p.bonusStats.additiveDamage = 10 / 100.f;

            return p;
        }

        static Passive SharpBlades()
        {
            auto p = passive("Sharp Blades", PassiveType::None);

            p.restrictWeapon = true;
            p.bonusStats.additiveDamage = 10 / 100.f;

            return p;
        }
    };

    struct Hammer : private Base<Weapon::Hammer, DmgType::Melee>
    {
        static Passive Thunderstruck()
        {
            auto p = passive("Thunderstruck", PassiveType::Elite);

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
            auto p = passive("Momentum", PassiveType::Skill);

            p.trigger = Trigger::Hit;
            p.effect = EffectSlot::MomentumStack;
            p.restrictWeapon = true;

            return p;
        }

        static Passive ImprovedStrikes()
        {
            auto p = passive("Improved Strikes", PassiveType::None);

            p.restrictSubType = SubType::Strike;
            p.bonusStats.additiveDamage = 10 / 100.f;

            return p;
        }

        static Passive WreckingCrew()
        {
            auto p = passive("Wrecking Crew", PassiveType::None);

            p.restrictSubType = SubType::Strike;
            p.bonusStats.addedCritPower = 20 / 100.f;

            return p;
        }

        static Passive KnuckleDown()
        {
            auto p = passive("Knuckle Down", PassiveType::None);

            p.restrictType = SkillType::Builder;
            p.bonusStats.additiveDamage = 10 / 100.f;

            return p;
        }

        static Passive Punishment()
        {
            auto p = passive("Punishment", PassiveType::None);

            p.bonusStats.critPowerRating = 200; // better than stacks

            return p;
        }

        static Passive Tenderising()
        {
            auto p = passive("Tenderising", PassiveType::None);

            p.trigger = Trigger::Hit;
            p.effect = EffectSlot::Tenderising;

            return p;
        }

        static Passive HardImpact()
        {
            auto p = passive("Hard Impact", PassiveType::None);

            p.restrictWeapon = true;
            p.bonusStats.additiveDamage = 10 / 100.f;

            return p;
        }
    };

    struct Fist : private Base<Weapon::Fist, DmgType::Melee>
    {
        static Passive Predator()
        {
            auto p = passive("Predator", PassiveType::None);

            p.restrictWeapon = true;
            p.bonusStats.additiveDamage = 10 / 100.f;

            return p;
        }
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

        static Passive ThickerThanWater()
        {
            auto p = passive("Thicker Than Water", PassiveType::None);

            p.restrictWeapon = true;
            p.bonusStats.additiveDamage = 10 / 100.f;

            return p;
        }
    };

    struct Chaos : private Base<Weapon::Chaos, DmgType::Magic>
    {
        static Passive Calamity()
        {
            auto p = passive("Calamity", PassiveType::Skill);

            p.restrictWeapon = true;
            p.trigger = Trigger::Hit;
            p.triggerChance = 0.15f * 0; // BUG: Not logged, not in ACT, uncool :(
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

        static Passive ChaosAdept()
        {
            auto p = passive("Chaos Adept", PassiveType::None);

            p.restrictWeapon = true;
            p.bonusStats.additiveDamage = 10 / 100.f;

            return p;
        }
    };

    struct Elemental : private Base<Weapon::Elemental, DmgType::Magic>
    {
        static Passive ElementalForce()
        {
            auto p = passive("Elemental Force", PassiveType::None);

            p.trigger = Trigger::FinishActivation;
            p.triggerOnDamaging = true;
            p.effect = EffectSlot::ElementalForceStacks;

            return p;
        }

        static Passive ElementalOverload()
        {
            auto p = passive("Elemental Overload", PassiveType::Skill);

            p.restrictWeapon = true;
            p.trigger = Trigger::Hit;
            p.triggerChance = 0.333f;
            p.effect = EffectSlot::ElementalOverload;

            return p;
        }

        static Passive LiveWire()
        {
            auto p = passive("Live Wire", PassiveType::Elite);

            p.trigger = Trigger::Crit;
            p.effect = EffectSlot::LiveWireStack;
            p.abilityBlockedEffect = EffectSlot::LiveWireProc;

            return p;
        }

        static Passive ViolentStrikes()
        {
            auto p = passive("Violent Strikes", PassiveType::None);

            p.restrictSubType = SubType::Strike;
            p.bonusStats.addedCritChance = 7.5f / 100.f;

            return p;
        }

        static Passive ElementalPrecision()
        {
            auto p = passive("Elemental Precision", PassiveType::None);

            p.restrictWeapon = true;
            p.bonusStats.additiveDamage = 10 / 100.f;

            return p;
        }
    };

    static Passive empty() { return Passive(); }

    static std::vector<Passive> all(); // only non-skills
private:
    Passives() = delete;
};
