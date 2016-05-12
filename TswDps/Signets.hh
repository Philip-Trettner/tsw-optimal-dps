#pragma once

#include "Signet.hh"

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
            p.effect.name = name;
            return p;
        }
    };

public:
    struct Major : private Base<SignetSlot::Major>
    {
        static Signet Violence()
        {
            auto p = signet("Violence");

            p.effect.bonusStats.attackRating = 141;

            return p;
        }
    };

    struct Minor : private Base<SignetSlot::Minor>
    {
        static Signet Issue1p5()
        {
            auto p = signet("Issue 1.5%");

            p.effect.bonusStats.multiplicativeDamage = 1.5f / 100;

            return p;
        }
    };

private:
    Signets() = delete;
};
