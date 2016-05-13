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

            return e;
        }
        static Effect Aggression()
        {
            auto e = effect("Aggression", EffectSlot::Aggression);

            e.cooldownIn60th = 9 * 60;
            e.timeIn60th = 6 * 60;
            e.bonusStats.additiveDamage = 15 / 100.f;

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
    };

    struct Passive : private Base
    {
        static Effect TwistTheKnife()
        {
            auto e = effect("Twist The Knife", EffectSlot::TwistTheKnife);

            e.timeIn60th = 10 * 60;
            e.maxStacks = 3;
            e.bonusStats.additiveDamage = 4 / 100.f;

            return e;
        }

        static Effect Lethality()
        {
            auto e = effect("Lethality", EffectSlot::Lethality);

            e.timeIn60th = 10 * 60;
            e.maxStacks = 10;
            e.bonusStats.additiveDamage = 1.25f / 100.f;

            return e;
        }
    };

private:
    Effects() = delete;
};
