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
        static Signet Fury()
        {
            auto p = signet("Fury");

            p.passive.trigger = Trigger::FinishActivation;
            p.passive.triggerOnDamaging = true;
            p.passive.effect = EffectSlot::FuryStacks;

            return p;
        }
        static Signet Sadism()
        {
            auto p = signet("Sadism");

            p.passive.trigger = Trigger::Hit; // TODO: Test me!
            p.passive.triggerChance = 0.1f;
            p.passive.effect = EffectSlot::Sadism;

            return p;
        }
        static Signet Opportunism()
        {
            auto p = signet("Opportunism");

            p.passive.trigger = Trigger::Hit; // TODO: Test me!
            p.passive.triggerChance = 0.2f;
            p.passive.effect = EffectSlot::Opportunism;

            return p;
        }

        static std::vector<Signet> all()
        {
            return {
                Laceration(),  //
                Abuse(),       //
                Aggression(),  //
                Sadism(),      //
                Opportunism(), //
                Fury(),        //
            };
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
        static Signet EgonPendant()
        {
            auto p = signet("Egon Pendant");

            p.passive.trigger = Trigger::Crit;
            p.passive.effect = EffectSlot::EgonPendant;

            return p;
        }
        static Signet ConeyIslandBand()
        {
            auto p = signet("Coney Island Band");

            p.passive.trigger = Trigger::CritHealAny;
            p.passive.effect = EffectSlot::ConeyIslandBand;

            return p;
        }

        static std::vector<Signet> all()
        {
            return {
                Violence(),         //
                WoodcuttersWrath(), //
                EgonPendant(),      //
                ConeyIslandBand(),  //
            };
        }
    };

    struct Minor : private Base<SignetSlot::Minor>
    {
        static Signet Equilibrium()
        {
            auto p = signet("Equilibrium");

            p.passive.trigger = Trigger::CritHealSelf;
            p.passive.effect = EffectSlot::Equilibrium;

            return p;
        }

        static Signet Issue1p5()
        {
            auto p = signet("Issue 1.5%");

            p.passive.bonusStats.multiplicativeDamage = 1.5f / 100;

            return p;
        }

        static Signet Assassination()
        {
            auto p = signet("Assassination");

            p.passive.restrictSubType = SubType::Strike;
            p.passive.bonusStats.additiveDamage = 4.5f / 100;

            return p;
        }

        static Signet Cleaving()
        {
            auto p = signet("Cleaving");

            p.passive.restrictSubType = SubType::Focus;
            p.passive.bonusStats.additiveDamage = 4.5f / 100;

            return p;
        }

        static Signet Storms()
        {
            auto p = signet("Storms");

            p.passive.restrictSubType = SubType::Burst;
            p.passive.bonusStats.additiveDamage = 4.5f / 100;

            return p;
        }

        static Signet Distortion()
        {
            auto p = signet("Distortion");

            p.passive.weaponType = Weapon::Chaos;
            p.passive.restrictWeapon = true;
            p.passive.bonusStats.additiveDamage = 3.0f / 100;

            return p;
        }

        static Signet Execution()
        {
            auto p = signet("Execution");

            p.passive.weaponType = Weapon::Rifle;
            p.passive.restrictWeapon = true;
            p.passive.bonusStats.additiveDamage = 3.0f / 100;

            return p;
        }

        static Signet Flux()
        {
            auto p = signet("Flux");

            p.passive.weaponType = Weapon::Elemental;
            p.passive.restrictWeapon = true;
            p.passive.bonusStats.additiveDamage = 3.0f / 100;

            return p;
        }

        static Signet Liquidation()
        {
            auto p = signet("Liquidation");

            p.passive.weaponType = Weapon::Pistol;
            p.passive.restrictWeapon = true;
            p.passive.bonusStats.additiveDamage = 3.0f / 100;

            return p;
        }

        static Signet Serration()
        {
            auto p = signet("Serration");

            p.passive.weaponType = Weapon::Fist;
            p.passive.restrictWeapon = true;
            p.passive.bonusStats.additiveDamage = 3.0f / 100;

            return p;
        }

        static Signet Shards()
        {
            auto p = signet("Shards");

            p.passive.weaponType = Weapon::Shotgun;
            p.passive.restrictWeapon = true;
            p.passive.bonusStats.additiveDamage = 3.0f / 100;

            return p;
        }

        static Signet Shattering()
        {
            auto p = signet("Shattering");

            p.passive.weaponType = Weapon::Hammer;
            p.passive.restrictWeapon = true;
            p.passive.bonusStats.additiveDamage = 3.0f / 100;

            return p;
        }

        static Signet Swords()
        {
            auto p = signet("Swords");

            p.passive.weaponType = Weapon::Blade;
            p.passive.restrictWeapon = true;
            p.passive.bonusStats.additiveDamage = 3.0f / 100;

            return p;
        }

        static Signet Tomes()
        {
            auto p = signet("Tomes");

            p.passive.weaponType = Weapon::Blood;
            p.passive.restrictWeapon = true;
            p.passive.bonusStats.additiveDamage = 3.0f / 100;

            return p;
        }

        static Signet SubwayTokens()
        {
            auto p = signet("Subway Tokens");

            p.passive.effect = EffectSlot::SubwayTokensCountdown;
            p.passive.trigger = Trigger::Auto;

            return p;
        }

        static std::vector<Signet> all()
        {
            return {
                Issue1p5(), //

                Assassination(), //
                Cleaving(),      //
                Storms(),        //

                Distortion(),  //
                Execution(),   //
                Flux(),        //
                Liquidation(), //
                Serration(),   //
                Shards(),      //
                Shattering(),  //
                Swords(),      //
                Tomes(),       //

                Equilibrium(), //
            };
        }
    };

    static std::vector<Signet> all()
    {
        auto hw = HeadWeapon::all();
        auto major = Major::all();
        auto minor = Minor::all();
        std::vector<Signet> s;
        s.insert(end(s), begin(hw), end(hw));
        s.insert(end(s), begin(major), end(major));
        s.insert(end(s), begin(minor), end(minor));
        s.push_back(Minor::SubwayTokens());
        return s;
    }

    static Signet empty() { return Signet(); }
private:
    Signets() = delete;
};
