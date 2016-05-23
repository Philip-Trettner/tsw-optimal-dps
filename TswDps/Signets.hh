#pragma once

#include "Signet.hh"
#include "Effects.hh"

struct Signets
{
private:
    template <SignetSlot slot>
    struct Base
    {
    protected:
        Base() = delete;

        static Signet signet(std::string const& name)
        {
            Signet p;
            p.slot = slot;
            p.passive.name = name;
            return p;
        }
    };

public:
    struct HeadWeapon : private Base<SignetSlot::HeadWeapon>
    {
        static Signet Laceration()
        {
            auto p = signet("Laceration");

            p.passive.trigger = Trigger::Crit;
            p.passive.effect = EffectSlot::Laceration;

            return p;
        }
        static Signet Abuse()
        {
            auto p = signet("Abuse");

            p.passive.trigger = Trigger::Hit;
            p.passive.triggerChance = 0.10f;
            p.passive.effect = EffectSlot::Abuse;

            return p;
        }
        static Signet Aggression()
        {
            auto p = signet("Aggression");

            p.passive.trigger = Trigger::Hit;
            p.passive.triggerChance = 0.15f;
            p.passive.effect = EffectSlot::Aggression;

            return p;
        }
    };

    struct Major : private Base<SignetSlot::Major>
    {
        static Signet Violence()
        {
            auto p = signet("Violence");

            p.passive.bonusStats.attackRating = 141;

            return p;
        }
        static Signet WoodcuttersWrath()
        {
            auto p = signet("Woodcutters Wrath");

            p.passive.trigger = Trigger::Hit;
            p.passive.effect = EffectSlot::MothersWrathStacks;

            return p;
        }
    };

    struct Minor : private Base<SignetSlot::Minor>
    {
        static Signet Issue1p5()
        {
            auto p = signet("Issue 1.5%");

            p.passive.bonusStats.multiplicativeDamage = 1.5f / 100;

            return p;
        }
    };

    static Signet empty()
    {
        return Signet();
    }

private:
    Signets() = delete;
};
