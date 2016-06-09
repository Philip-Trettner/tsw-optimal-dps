#pragma once

#include "Augment.hh"

struct Augments
{
    //////////////////////////////////////// DPS
    static Augment Brutal()
    {
        Augment a{"Brutal"};
        a.bonusStats.addedCritChance = .075f;
        return a;
    }
    static Augment Fierce()
    {
        Augment a{"Fierce"};
        a.bonusStats.addedCritChance = .05f;
        a.bonusStats.addedPenChance = .05f;
        return a;
    }
    static Augment Piercing()
    {
        Augment a{"Piercing"};
        a.bonusStats.addedPenChance = .075f;
        return a;
    }
    static Augment Ferocious()
    {
        Augment a{"Ferocious"};
        a.bonusStats.addedCritPower = .15f;
        return a;
    }
    static Augment Focused()
    {
        Augment a{"Focused"};
        a.bonusStats.addedCritChance = .05f;
        a.bonusStats.addedCritPower = .10f;
        return a;
    }
    static Augment Precise()
    {
        Augment a{"Precise"};
        a.bonusStats.hitRating = 250;
        return a;
    }
    static Augment Grievous()
    {
        Augment a{"Grievous"};
        // a.bonusStats.additiveDamage = .05f;
        a.bonusStats.additiveDamage = .15f; // I'm a scrub.
        return a;
    }
    static Augment Accurate()
    {
        Augment a{"Accurate"};
        a.bonusStats.attackRating = 150;
        a.affectEverything = true;
        return a;
    }

    static std::vector<Augment> allDpsAugs()
    {
        return {
            Accurate(),  //
            Brutal(),    //
            Precise(),   //
            Piercing(),  //
            Ferocious(), //
            Fierce(),    //
            Focused(),   //
            Grievous()   //
        };
    }

    //////////////////////////////////////// Support

    static Augment Curing()
    {
        Augment a{"Curing"};
        a.effect = EffectSlot::AugCuring;
        return a;
    }
    static Augment Inspiring()
    {
        Augment a{"Inspiring"};
        a.effect = EffectSlot::AugInspiring;
        return a;
    }
    static Augment Accelerating()
    {
        Augment a{"Accelerating"};
        a.effect = EffectSlot::AugAccelerating;
        return a;
    }
    static Augment Quickening()
    {
        Augment a{"Quickening"};
        a.effect = EffectSlot::AugQuickening;
        return a;
    }
    static Augment Invulnerable()
    {
        Augment a{"Invulnerable"};
        a.effect = EffectSlot::AugInvulnerable;
        return a;
    }
    static Augment Salubrious()
    {
        Augment a{"Salubrious"};
        a.effect = EffectSlot::AugSalubrious;
        return a;
    }
    static Augment Mercurial()
    {
        Augment a{"Mercurial"};
        a.effect = EffectSlot::AugMercurial;
        return a;
    }

    static std::vector<Augment> allSupportAugs()
    {
        return {
            Curing(),       //
            Inspiring(),    //
            Accelerating(), //
            Quickening(),   //
            Invulnerable(), //
            Salubrious(),   //
            Mercurial(),    //
        };
    }

    //////////////////////////////////////// Other
    static Augment Mending()
    {
        Augment a{"Mending"};
        a.slot = AugmentSlot::Heal;
        a.bonusStats.healRating = 150;
        a.affectEverything = true;
        return a;
    }
    static Augment Robust()
    {
        Augment a{"Robust"};
        a.slot = AugmentSlot::Survival;
        a.bonusStats.health = 250;
        a.affectEverything = true;
        return a;
    }
    static Augment empty() { return {}; }
private:
    Augments() = delete;
};
