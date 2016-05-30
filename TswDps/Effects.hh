#pragma once

#include "Effect.hh"

#include "SkillTable.hh"

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
    static float scaling(std::string const& name) { return SkillTable::scaling(name); }

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

        static Effect MothersWrathStacks()
        {
            auto e = effect("Mother's Wrath Stacks", EffectSlot::MothersWrathStacks);

            e.timeIn60th = INF_TIME;
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

        static Effect EgonPendant()
        {
            auto e = effect("Egon Pendant", EffectSlot::EgonPendant);

            e.timeIn60th = 12 * 60;
            e.cooldownIn60th = 12 * 60;
            e.bonusStats.egonPendant = true;

            return e;
        }

        static Effect FuryStacks()
        {
            auto e = effect("Fury Stacks", EffectSlot::FuryStacks);

            e.timeIn60th = INF_TIME;
            e.maxStacks = 10;
            e.cooldownIn60th = 0; // no CD (after dmg ability)
            e.triggerOnMaxStacks = EffectSlot::FuryBuff;
            e.blockedSlot = EffectSlot::FuryBuff;

            return e;
        }

        static Effect FuryBuff()
        {
            auto e = effect("Fury Buff", EffectSlot::FuryBuff);

            e.timeIn60th = 6 * 60;
            e.cooldownIn60th = 6 * 60;
            e.bonusStats.additiveDamage = 15 / 100.f;

            return e;
        }

        static Effect Sadism()
        {
            auto e = effect("Sadism", EffectSlot::Sadism);

            e.dmgtype = DmgType::None;
            e.procDmgFixed = 427;

            return e;
        }

        static Effect Opportunism()
        {
            auto e = effect("Opportunism", EffectSlot::Opportunism);

            e.dmgtype = DmgType::None;
            e.procDmgFixed = 224;

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
            e.procDmgScaling = scaling(e.name);

            return e;
        }

        static Effect ElementalOverload()
        {
            auto e = effect("Elemental Overload", EffectSlot::ElementalOverload);

            e.dmgtype = DmgType::Magic;
            e.procDmgScaling = scaling(e.name);

            return e;
        }

        static Effect MomentumStack()
        {
            auto e = effect("Momentum Stack", EffectSlot::MomentumStack);

            e.timeIn60th = INF_TIME;
            e.maxStacks = 5;
            e.triggerOnMaxStacks = EffectSlot::MomentumBuff;
            e.blockedSlot = EffectSlot::MomentumBuff;
            e.restrictToWeapon = Weapon::Hammer;
            e.affectProcs = false;

            return e;
        }

        static Effect MomentumBuff()
        {
            auto e = effect("Momentum Buff", EffectSlot::MomentumBuff);

            e.timeIn60th = INF_TIME;
            e.bonusStats.additiveDamage = 40 / 100.f;
            e.restrictToWeapon = Weapon::Hammer;
            e.consumedAfterHit = true;

            return e;
        }

        static Effect BloodOffering()
        {
            auto e = effect("Blood Offering", EffectSlot::BloodOffering);

            e.timeIn60th = 8 * 60;
            e.bonusStats.additiveDamage = 11.5f / 100.f;
            e.restrictToWeapon = Weapon::Blood;

            return e;
        }

        static Effect ElementalFury()
        {
            auto e = effect("Elemental Fury", EffectSlot::ElementalFury);

            e.timeIn60th = 5 * 60;
            e.bonusStats.additiveDamage = 7.5f / 100.f;

            return e;
        }
    };

    struct Proc : private Base
    {
        static Effect Thunderstruck()
        {
            auto e = effect("Thunderstruck", EffectSlot::Thunderstruck);

            e.dmgtype = DmgType::Melee;
            e.procDmgScaling = scaling(e.name);

            return e;
        }
        static Effect SuddenReturn()
        {
            auto e = effect("Sudden Return", EffectSlot::SuddenReturn);

            e.dmgtype = DmgType::Melee;
            e.procDmgScaling = scaling(e.name);

            return e;
        }
        static Effect Tenderising()
        {
            auto e = effect("Tenderising", EffectSlot::Tenderising);

            e.dmgtype = DmgType::Melee;
            e.procDmgScaling = scaling(e.name);

            return e;
        }
        static Effect FortunateStrike()
        {
            auto e = effect("Fortunate Strike", EffectSlot::FortunateStrike);

            e.dmgtype = DmgType::Melee;
            e.procDmgScaling = scaling(e.name);

            return e;
        }
        static Effect OneInTheChamber()
        {
            auto e = effect("One in the Chamber", EffectSlot::OneInTheChamber);

            e.dmgtype = DmgType::Ranged;
            e.procDmgScaling = scaling(e.name);

            return e;
        }
        static Effect Gnosis()
        {
            auto e = effect("Gnosis", EffectSlot::Gnosis);

            e.dmgtype = DmgType::Magic;
            e.procDmgScaling = scaling(e.name);

            return e;
        }
        static Effect GrandSlam()
        {
            auto e = effect("Grand Slam Proc", EffectSlot::GrandSlam);

            e.dmgtype = DmgType::Melee;
            e.procDmgScaling = scaling(e.name);
            e.affectedByAdditiveDmg = true; // TODO: TEST ME!

            return e;
        }

        static Effect LiveWireStack()
        {
            auto e = effect("Live Wire Stack", EffectSlot::LiveWireStack);

            e.cooldownIn60th = 0;
            e.timeIn60th = 9 * 60;
            e.consumedAfterHit = true;
            e.gainOnConsume = EffectSlot::LiveWireProc;
            e.cannotConsumeSameAbility = true;

            return e;
        }

        static Effect LiveWireProc()
        {
            auto e = effect("Live Wire Proc", EffectSlot::LiveWireProc);

            e.dmgtype = DmgType::Magic;
            e.procDmgScaling = scaling(e.name);

            return e;
        }
    };

    struct Dots : private Base
    {
        static Effect Bombardment()
        {
            auto e = effect("Bombardment", EffectSlot::Bombardment);

            e.timeIn60th = 2 * 60 + 30;
            e.maxStacks = 8;

            e.procOn = ProcOn::Loss;
            e.dmgtype = DmgType::Ranged;
            e.procDmgScaling = scaling(e.name);
            e.affectedByAdditiveDmg = true;

            return e;
        }

        static Effect Whiteout()
        {
            auto e = effect("Whiteout", EffectSlot::Whiteout);

            e.timeIn60th = 30;
            e.maxStacks = 16;

            e.procOn = ProcOn::Loss;
            e.dmgtype = DmgType::Magic;
            e.procDmgScaling = scaling(e.name);
            e.affectedByAdditiveDmg = true;

            return e;
        }

        static Effect Plague()
        {
            auto e = effect("Plague", EffectSlot::Plague);

            e.timeIn60th = 2 * 60;
            e.maxStacks = 6;

            e.procOn = ProcOn::Loss;
            e.dmgtype = DmgType::Magic;
            e.procDmgScaling = scaling(e.name);
            e.affectedByAdditiveDmg = true;

            return e;
        }

        static Effect LeftHandOfDarkness()
        {
            auto e = effect("Left Hand of Darkness", EffectSlot::LeftHandOfDarkness);

            e.timeIn60th = 60;
            e.maxStacks = 4;

            e.procOn = ProcOn::Loss;
            e.dmgtype = DmgType::Magic;
            e.procDmgScaling = scaling(e.name);
            e.affectedByAdditiveDmg = true; // TODO: Test me!

            return e;
        }

        static Effect Contaminate()
        {
            auto e = effect("Contaminate", EffectSlot::Contaminate);

            e.timeIn60th = 60;
            e.maxStacks = 6;

            e.procOn = ProcOn::Loss;
            e.dmgtype = DmgType::Magic;
            e.procDmgScaling = scaling(e.name);
            e.affectedByAdditiveDmg = true; // TODO: Test me!

            return e;
        }

        static Effect GoForTheThroat()
        {
            auto e = effect("Go for the Throat Afflict", EffectSlot::GoForTheThroat);

            e.timeIn60th = 60;
            e.maxStacks = 10;

            e.procOn = ProcOn::Loss;
            e.dmgtype = DmgType::Melee;
            e.procDmgScaling = scaling(e.name);
            e.affectedByAdditiveDmg = true;

            return e;
        }

        static Effect EyeOfPandemonium()
        {
            auto e = effect("Eye of Pandemonium 2ndary", EffectSlot::EyeOfPandemonium);

            e.timeIn60th = 60;
            e.maxStacks = 10;

            e.procOn = ProcOn::Loss;
            e.dmgtype = DmgType::Magic;
            e.procDmgScaling = scaling(e.name);
            e.affectedByAdditiveDmg = true;

            return e;
        }

        static Effect PowerLine()
        {
            auto e = effect("Power Line", EffectSlot::PowerLine);

            e.timeIn60th = 60;
            e.maxStacks = 10;

            e.procOn = ProcOn::Loss;
            e.dmgtype = DmgType::Magic;
            e.procDmgScaling = scaling(e.name);
            e.affectedByAdditiveDmg = true;

            e.triggerOnStackLostOnlyLast = true;
            e.triggerOnStackLost = EffectSlot::PowerLineDetonation;

            return e;
        }

        static Effect PowerLineDetonation()
        {
            auto e = effect("Power Line Detonation", EffectSlot::PowerLineDetonation);

            e.dmgtype = DmgType::Magic;
            e.procDmgScaling = scaling(e.name) * 2.8f; // assume always max bonus (3 is unrealistic)
            e.affectedByAdditiveDmg = true;
            e.isFullHit = true; // TODO: TEST!

            return e;
        }

        static Effect FireManifestation()
        {
            auto e = effect("Fire Manifestation", EffectSlot::FireManifestation);

            e.timeIn60th = 2 * 60 + 30;
            e.maxStacks = 4;

            e.procOn = ProcOn::Loss;
            e.dmgtype = DmgType::Magic;
            e.procDmgScaling = scaling(e.name);
            e.affectedByAdditiveDmg = true;
            e.isFullHit = true;

            e.triggerOnStackLost = EffectSlot::ElementalFury;

            return e;
        }

        static Effect LightningManifestation()
        {
            auto e = effect("Lightning Manifestation", EffectSlot::LightningManifestation);

            e.timeIn60th = 60;
            e.maxStacks = 10;

            e.procOn = ProcOn::Loss;
            e.dmgtype = DmgType::Magic;
            e.procDmgScaling = scaling(e.name);
            e.affectedByAdditiveDmg = true;
            e.isFullHit = true;

            e.triggerOnStackLost = EffectSlot::ElementalFury;

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

        static Effect ElementalForceStacks()
        {
            auto e = effect("Elemental Force Stacks", EffectSlot::ElementalForceStacks);

            e.timeIn60th = INF_TIME;
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

        static Effect FatalFlourishStacks()
        {
            auto e = effect("Fatal Flourish Stacks", EffectSlot::FatalFlourishStacks);

            e.timeIn60th = INF_TIME;
            e.maxStacks = 5;
            e.cooldownIn60th = 0; // no CD (on finish activation)
            e.triggerOnMaxStacks = EffectSlot::FatalFlourishBuff;
            e.blockedSlot = EffectSlot::FatalFlourishBuff;

            return e;
        }

        static Effect FatalFlourishBuff()
        {
            auto e = effect("Fatal Flourish Buff", EffectSlot::FatalFlourishBuff);

            e.timeIn60th = 5 * 60;
            e.cooldownIn60th = 5 * 60;
            e.bonusStats.addedPenChance = 20 / 100.f;

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

            return e;
        }
        static Effect AmorFati()
        {
            auto e = effect("Amor Fati", EffectSlot::AmorFati);

            e.timeIn60th = 10 * 60;
            e.bonusStats.additiveDamage = 10 / 100.f;

            return e;
        }
        static Effect TearEmUp()
        {
            auto e = effect("Tear Em Up", EffectSlot::TearEmUp);

            e.timeIn60th = 4 * 60;
            e.bonusStats.additiveDamage = 10 / 100.f;

            return e;
        }
        static Effect GunFu()
        {
            auto e = effect("Gun-Fu", EffectSlot::GunFu);

            e.gainResources = 5;

            return e;
        }
        static Effect LockAndLoad()
        {
            auto e = effect("Lock & Load", EffectSlot::LockAndLoad);

            e.gainResources = 3;

            return e;
        }
        static Effect Cannibalize()
        {
            auto e = effect("Cannibalize", EffectSlot::Cannibalize);

            e.gainResources = 3;

            e.timeIn60th = 20 * 60;
            e.restrictToSkillType = SkillType::Consumer;
            e.restrictToWeapon = Weapon::Blood;
            e.consumedAfterAbility = true;
            e.bonusStats.additiveDamage = 20 / 100.f;

            return e;
        }

        static Effect FullMomentum()
        {
            auto e = effect("Full Momentum", EffectSlot::FullMomentum);

            e.gainResources = 5;
            e.gainEffectStacks = EffectSlot::MomentumStack;
            e.gainEffectStacksTo = 5;

            return e;
        }

        static Effect LockStockBarrel()
        {
            auto e = effect("Lock, Stock & Barrel", EffectSlot::LockStockBarrel);

            e.timeIn60th = 20 * 60;
            e.consumedAfterAbility = true;
            e.gainOnConsume = EffectSlot::LockStockBarrelGain;
            e.restrictToWeapon = Weapon::Shotgun;
            e.restrictToSkillType = SkillType::Consumer;

            return e;
        }

        static Effect LockStockBarrelGain()
        {
            auto e = effect("Lock, Stock & Barrel (Gain)", EffectSlot::LockStockBarrelGain);

            e.gainResources = 5;

            return e;
        }


        static Effect SteelEcho()
        {
            auto e = effect("Steel Echo", EffectSlot::SteelEcho);

            e.timeIn60th = 10 * 60;
            e.restrictToWeapon = Weapon::Blade;

            e.dmgtype = DmgType::Melee;
            e.procDmgScaling = scaling(e.name);
            e.procOn = ProcOn::SkillHit;
            e.isFullHit = true;

            return e;
        }

    };

private:
    Effects() = delete;
};
