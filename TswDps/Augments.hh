#pragma once

#include "Augment.hh"

struct Augments
{
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
    static Augment Grievous()
    {
        Augment a{"Grievous"};
        a.bonusStats.additiveDamage = .15f;
        return a;
    }
    static Augment Robust()
    {
        Augment a{"Robust"};
        a.bonusStats.health = 250;
        a.affectEverything = true;
        return a;
    }
    static Augment Accurate()
    {
        Augment a{"Accurate"};
        a.bonusStats.attackRating = 150;
        a.affectEverything = true;
        return a;
    }
    static Augment Mending()
    {
        Augment a{"Mending"};
        a.bonusStats.healRating = 150;
        a.affectEverything = true;
        return a;
    }
    static Augment empty() { return {}; }
private:
    Augments() = delete;
};
