#pragma once

#include "Augment.hh"

struct Augments
{
    static Augment Brutal()
    {
        Augment a{"Brutal"};
        a.addCritChance = .075;
        return a;
    }
    static Augment Fierce()
    {
        Augment a{"Fierce"};
        a.addCritChance = .05;
        a.addPenChance = .05;
        return a;
    }
    static Augment Piercing()
    {
        Augment a{"Piercing"};
        a.addPenChance = .075;
        return a;
    }
    static Augment Ferocious()
    {
        Augment a{"Ferocious"};
        a.addCritDamage = .15;
        return a;
    }
    static Augment Grievous()
    {
        Augment a{"Grievous"};
        a.addDamage = .15;
        return a;
    }
    static Augment Robust()
    {
        Augment a{"Robust"};
        a.addHP = 250;
        return a;
    }
    static Augment Accurate()
    {
        Augment a{"Accurate"};
        a.addAttackRating = 150;
        return a;
    }
    static Augment Mending()
    {
        Augment a{"Mending"};
        a.addHealRating = 150;
        return a;
    }
    static Augment empty() { return {}; }
private:
    Augments() = delete;
};