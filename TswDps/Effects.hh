#pragma once

#include "Effect.hh"

struct Effects
{
private:
    struct Base
    {
    protected:
        Base() = delete;

        static Effect effect(std::string const& name, EffectSlot slot)
        {
            Effect e;
            e.name = name;
            e.slot = slot;
            return e;
        }
    };

public:
    struct Signet : private Base
    {
        static Effect Laceration()
        {
            auto e = effect("Laceration", EffectSlot::Laceration);

            e.cooldownIn60th = 15 * 60;
            e.timeIn60th = 15 * 60;
            e.bonusStats.addedCritPower = 24 / 100.f;

            return e;
        }
        static Effect Abuse()
        {
            auto e = effect("Abuse", EffectSlot::Abuse);

            e.oncePerAbility = true;
            e.cooldownIn60th = 11 * 60;
            e.timeIn60th = 5 * 60;
            e.bonusStats.additiveDamage = 24 / 100.f;

            e.affectProcs = false; // ??? TEST ME

            return e;
        }
        static Effect Aggression()
        {
            auto e = effect("Aggression", EffectSlot::Aggression);

            e.cooldownIn60th = 9 * 60;
            e.timeIn60th = 6 * 60;
            e.bonusStats.additiveDamage = 15 / 100.f;

            e.affectProcs = false; // ??? TEST ME

            return e;
        }

        static Effect MothersWrathStacks()
        {
            auto e = effect("Mother's Wrath Stacks", EffectSlot::MothersWrathStacks);

            e.timeIn60th = 60 * 60; // dummy
            e.maxStacks = 5;
            e.resetOnPen = true;
            e.cooldownIn60th = 0; // no CD
            e.triggerOnMaxStacks = EffectSlot::MothersWrathBuff;
            e.blockedSlot = EffectSlot::MothersWrathBuff;

            return e;
        }

        static Effect MothersWrathBuff()
        {
            auto e = effect("Mother's Wrath Buff", EffectSlot::MothersWrathBuff);

            e.timeIn60th = 3 * 60;
            e.cooldownIn60th = 6 * 60;
            e.bonusStats.addedPenChance = 40 / 100.f;

            return e;
        }
    };

    struct Generic : private Base
    {
        static Effect MinorPenetrationChance()
        {
            auto e = effect("Minor Penetration Chance", EffectSlot::MinorPenetrationChance);

            e.timeIn60th = 8 * 60;
            e.bonusStats.addedPenChance = 15 / 100.f;

            return e;
        }
        static Effect MinorCriticalChance()
        {
            auto e = effect("Minor Critical Chance", EffectSlot::MinorCriticalChance);

            e.timeIn60th = 8 * 60;
            e.bonusStats.addedCritChance = 10 / 100.f;

            return e;
        }
        static Effect MajorPenetrationChance()
        {
            auto e = effect("Major Penetration Chance", EffectSlot::MajorPenetrationChance);

            e.timeIn60th = 8 * 60;
            e.bonusStats.addedPenChance = 45 / 100.f;

            return e;
        }
        static Effect MajorCriticalChance()
        {
            auto e = effect("Major Critical Chance", EffectSlot::MajorCriticalChance);

            e.timeIn60th = 10 * 60;
            e.bonusStats.addedCritChance = 40 / 100.f;

            return e;
        }
    };

    struct WeaponSkill : private Base
    {
        static Effect DoubleUp()
        {
            auto e = effect("Double Up", EffectSlot::DoubleUp);

            e.dmgtype = DmgType::Ranged;
            e.procDmgPercentage = 0.50f;

            return e;
        }

        static Effect Calamity()
        {
            auto e = effect("Calamity", EffectSlot::Calamity);

            e.dmgtype = DmgType::Magic;
            e.procDmgScaling = 0.47749f;

            return e;
        }

        static Effect ElementalOverload()
        {
            auto e = effect("Elemental Overload", EffectSlot::ElementalOverload);

            e.dmgtype = DmgType::Magic;
            e.procDmgScaling = 0.13600f;

            return e;
        }
    };

    struct Proc : private Base
    {
        static Effect Thunderstruck()
        {
            auto e = effect("Thunderstruck", EffectSlot::Thunderstruck);

            e.dmgtype = DmgType::Melee;
            e.procDmgScaling = 0.55195f;

            return e;
        }
        static Effect SuddenReturn()
        {
            auto e = effect("Sudden Return", EffectSlot::SuddenReturn);

            e.dmgtype = DmgType::Melee;
            e.procDmgScaling = 0.36830f;

            return e;
        }
        static Effect FortunateStrike()
        {
            auto e = effect("Fortunate Strike", EffectSlot::FortunateStrike);

            e.dmgtype = DmgType::Melee;
            e.procDmgScaling = 0.92024f;

            return e;
        }
        static Effect OneInTheChamber()
        {
            auto e = effect("One In The Chamber", EffectSlot::OneInTheChamber);

            e.dmgtype = DmgType::Ranged;
            e.procDmgScaling = 0.36830f;

            return e;
        }
        static Effect Gnosis()
        {
            auto e = effect("Gnosis", EffectSlot::Gnosis);

            e.dmgtype = DmgType::Magic;
            e.procDmgScaling = 0.27895f;

            return e;
        }
    };

    struct Passive : private Base
    {
        static Effect TwistTheKnife()
        {
            auto e = effect("Twist The Knife", EffectSlot::TwistTheKnife);

            e.timeIn60th = 10 * 60;
            e.maxStacks = 3;
            e.bonusStats.additiveDamage = 4 / 100.f;

            e.affectProcs = false; // ??? TEST ME

            return e;
        }

        static Effect Lethality()
        {
            auto e = effect("Lethality", EffectSlot::Lethality);

            e.timeIn60th = 10 * 60;
            e.maxStacks = 10;
            e.bonusStats.additiveDamage = 1.25f / 100.f;

            e.affectProcs = false; // ??? TEST ME

            return e;
        }

        static Effect ElementalForceStacks()
        {
            auto e = effect("Elemental Force Stacks", EffectSlot::ElementalForceStacks);

            e.timeIn60th = 60 * 60; // dummy
            e.maxStacks = 5;
            e.cooldownIn60th = 0; // no CD (on finish activation)
            e.triggerOnMaxStacks = EffectSlot::ElementalForceBuff;
            e.blockedSlot = EffectSlot::ElementalForceBuff;

            return e;
        }

        static Effect ElementalForceBuff()
        {
            auto e = effect("Elemental Force Buff", EffectSlot::ElementalForceBuff);

            e.timeIn60th = 5 * 60;
            e.cooldownIn60th = 5 * 60;
            e.bonusStats.addedCritChance = 20 / 100.f;

            return e;
        }
    };

    struct SkillPassive : private Base
    {
        static Effect Reckless()
        {
            auto e = effect("Reckless", EffectSlot::Reckless);

            e.timeIn60th = 10 * 60;
            e.bonusStats.additiveDamage = 20 / 100.f;

            e.affectProcs = false; // ??? TEST ME

            return e;
        }
        static Effect AmorFati()
        {
            auto e = effect("Amor Fati", EffectSlot::AmorFati);

            e.timeIn60th = 10 * 60;
            e.bonusStats.additiveDamage = 10 / 100.f;

            e.affectProcs = false; // ??? TEST ME

            return e;
        }
    };

private:
    Effects() = delete;
};
