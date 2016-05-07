#pragma once

#include "Stats.hh"

struct Potions
{
    struct PureAnima
    {
        static Stats BenefactionTonic()
        {
            Stats s;
            s.attackRating = 120;
            s.healRating = 120;
            s.health = 470; // ???
            return s;
        }
        static Stats Attack()
        {
            Stats s;
            s.attackRating = 240;
            return s;
        }
        static Stats Heal()
        {
            Stats s;
            s.healRating = 240;
            return s;
        }
        static Stats HP()
        {
            Stats s;
            s.health = 940;
            return s;
        }

    private:
        PureAnima() = delete;
    };

    struct Anima
    {
        static Stats CritRating()
        {
            Stats s;
            s.critRating = 100;
            return s;
        }
        static Stats CritPowerRating()
        {
            Stats s;
            s.critPowerRating = 100;
            return s;
        }
        static Stats PenRating()
        {
            Stats s;
            s.penRating = 100;
            return s;
        }
        static Stats HitRating()
        {
            Stats s;
            s.hitRating = 100;
            return s;
        }

        static Stats BlockRating()
        {
            Stats s;
            s.blockRating = 100;
            return s;
        }

    private:
        Anima() = delete;
    };

private:
    Potions() = delete;
};
