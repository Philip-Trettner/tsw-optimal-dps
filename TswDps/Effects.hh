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

        static Effect SubwayTokens()
        {
            auto e = effect("Subway Tokens", EffectSlot::SubwayTokens);

            e.timeIn60th = INF_TIME;
            e.maxStacks = 11;
            e.resetOnGlance = true;
            e.resetOnMax = true;
            e.bonusStats.additiveDamage = 1 / 100.f;

            return e;
        }

        static Effect SubwayTokensCountdown()
        {
            auto e = effect("Subway Tokens Countdown", EffectSlot::SubwayTokensCountdown);

            e.timeIn60th = 6 * 60;
            e.triggerOnStackLost = EffectSlot::SubwayTokens;

            return e;
        }

        static Effect Equilibrium()
        {
            auto e = effect("Equilibrium", EffectSlot::Equilibrium);

            e.timeIn60th = 5 * 60;
            e.cooldownIn60th = 15 * 60;
            e.bonusStats.additiveDamage = 15 / 100.f;

            return e;
        }

        static Effect ConeyIslandBand()
        {
            auto e = effect("Coney Island Band", EffectSlot::ConeyIslandBand);

            e.timeIn60th = 15 * 60;
            e.cooldownIn60th = 15 * 60;
            e.bonusStats.addedCritPower = 25 / 100.f;

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
        static Effect MajorHitChance()
        {
            auto e = effect("Major Hit Chance", EffectSlot::MajorHitChance);

            e.timeIn60th = 5 * 60;
            e.bonusStats.reducedGlanceChance = 25 / 100.f;
            e.cooldownIn60th = e.timeIn60th; // cannot reapply

            return e;
        }
        static Effect CritRating()
        {
            auto e = effect("Critical Rating", EffectSlot::CritRating);

            e.cooldownIn60th = 0; // CHECK ME with Bullet Ballet!
            e.timeIn60th = 10 * 60;
            e.bonusStats.critRating = 40;
            e.maxStacks = 5;

            return e;
        }
        static Effect CritPowerRating()
        {
            auto e = effect("Crit Power Rating", EffectSlot::CritPowerRating);

            e.timeIn60th = 10 * 60;
            e.bonusStats.critPowerRating = 40;
            e.maxStacks = 5;

            return e;
        }
        static Effect PenRating()
        {
            auto e = effect("Penetration Rating", EffectSlot::PenRating);

            e.timeIn60th = 10 * 60;
            e.bonusStats.penRating = 40;
            e.maxStacks = 5;

            return e;
        }
        static Effect HitRating()
        {
            auto e = effect("Hit Rating", EffectSlot::HitRating);

            e.timeIn60th = 10 * 60;
            e.bonusStats.hitRating = 40;
            e.maxStacks = 5;

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
            e.procDmgScaling
                = scaling(e.name) * 2.7f; // assume always max bonus (3 is unrealistic, between 2.6 and 2.8)
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

        static Effect Ferocity()
        {
            auto e = effect("Ferocity", EffectSlot::Ferocity);

            e.timeIn60th = 5 * 60;
            e.maxStacks = 3;
            e.bonusStats.additiveDamage = 4 / 100.f;

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

        static Effect DoomStacks()
        {
            auto e = effect("Doom Stacks", EffectSlot::DoomStacks);

            // implicit: max once per sec
            e.timeIn60th = INF_TIME;
            e.maxStacks = 4;
            e.triggerOnMaxStacks = EffectSlot::DoomBuff;
            e.blockedSlot = EffectSlot::DoomBuff;

            return e;
        }

        static Effect DoomBuff()
        {
            auto e = effect("Doom Buff", EffectSlot::DoomBuff);

            e.timeIn60th = 20 * 60;
            e.bonusStats.addedPenChance = 1; // guaranteed pen
            e.affectProcs = false;           // only actual skills
            e.consumedAfterAbility = true;

            return e;
        }

        static Effect InvasiveMeasures()
        {
            auto e = effect("Invasive Measures", EffectSlot::InvasiveMeasures);

            e.cooldownIn60th = 0; // no ICD
            e.gainResources = 1;
            e.triggerOnMaxRes = EffectSlot::InvasiveMeasuresProc;

            return e;
        }

        static Effect InvasiveMeasuresProc()
        {
            auto e = effect("Invasive Measures Proc", EffectSlot::InvasiveMeasuresProc);

            e.cooldownIn60th = 0; // no ICD
            e.dmgtype = DmgType::Ranged;
            e.procDmgScaling = scaling(e.name);
            e.affectedByAdditiveDmg = true; // TODO: test me!

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
        static Effect AnimaCharge()
        {
            auto e = effect("Anima Charge", EffectSlot::AnimaCharge);

            e.timeIn60th = 20 * 60;
            e.restrictToSkillType = SkillType::Consumer;
            e.restrictToWeapon = Weapon::Elemental;
            e.consumedAfterAbility = true;
            e.makeConsumerFree = true;

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

    struct Augments : private Base
    {
        static Effect Curing()
        {
            auto e = effect("Augment Curing", EffectSlot::AugCuring);

            e.cooldownIn60th = 10 * 60;
            e.triggersGroupHealOnGain = true;

            return e;
        }

        static Effect Inspiring()
        {
            auto e = effect("Augment Inspiring", EffectSlot::AugInspiring);

            e.timeIn60th = 5 * 60;
            e.cooldownIn60th = 30 * 60;
            e.bonusStats.additiveDamage = 5 / 100.f;

            return e;
        }

        static Effect Accelerating()
        {
            auto e = effect("Augment Accelerating", EffectSlot::AugAccelerating);

            e.cooldownIn60th = 5 * 60;
            e.reducesCooldown = 5 / 100.f;

            return e;
        }

        static Effect Quickening()
        {
            auto e = effect("Augment Quickening", EffectSlot::AugQuickening);

            e.timeIn60th = 5 * 60;
            e.cooldownIn60th = 30 * 60;
            e.bonusStats.additiveDamage = 3 / 100.f;
            e.triggersGroupHealOnGain = true;

            return e;
        }

        static Effect Invulnerable()
        {
            auto e = effect("Augment Invulnerable", EffectSlot::AugInvulnerable);

            e.cooldownIn60th = 30 * 60;
            e.triggersGroupHealOnGain = true;

            return e;
        }

        static Effect Salubrious()
        {
            auto e = effect("Augment Salubrious", EffectSlot::AugSalubrious);

            e.cooldownIn60th = 30 * 60;
            e.triggersGroupHealOnGain = true;

            return e;
        }

        static Effect Mercurial()
        {
            auto e = effect("Augment Mercurial", EffectSlot::AugMercurial);

            e.cooldownIn60th = 10 * 60;
            e.reducesCooldown = 10 / 100.f;

            return e;
        }
    };

    struct Stimulants : private Base
    {
        static Effect StimAttackPurple()
        {
            auto e = effect("Stimulant Attack (purple)", EffectSlot::StimAttackPurple);

            e.timeIn60th = 20 * 60;
            e.bonusStats.attackRating = 600;
            // blue gives 78 and is therefore useless

            return e;
        }
        static Effect StimCritPurple()
        {
            auto e = effect("Stimulant Crit (purple)", EffectSlot::StimCritPurple);

            e.timeIn60th = 20 * 60;
            e.bonusStats.critRating = 341;

            return e;
        }
        static Effect StimCritBlue()
        {
            auto e = effect("Stimulant Crit (blue)", EffectSlot::StimCritBlue);

            e.timeIn60th = 20 * 60;
            e.bonusStats.critRating = 276;

            return e;
        }
        static Effect StimPenPurple()
        {
            auto e = effect("Stimulant Pen (purple)", EffectSlot::StimPenPurple);

            e.timeIn60th = 20 * 60;
            e.bonusStats.penRating = 312;

            return e;
        }
        static Effect StimPenBlue()
        {
            auto e = effect("Stimulant Pen (blue)", EffectSlot::StimPenBlue);

            e.timeIn60th = 20 * 60;
            e.bonusStats.penRating = 232;

            return e;
        }
    };

    struct Kickbacks : private Base
    {
        static Effect KickbackCritPurple()
        {
            auto e = effect("Kickback Crit (purple)", EffectSlot::KickbackCritPurple);

            e.timeIn60th = 4 * 60;
            e.cooldownIn60th = e.timeIn60th;
            e.bonusStats.critRating = 712;

            return e;
        }
        static Effect KickbackCritBlue()
        {
            auto e = effect("Kickback Crit (blue)", EffectSlot::KickbackCritBlue);

            e.timeIn60th = 4 * 60;
            e.cooldownIn60th = e.timeIn60th;
            e.bonusStats.critRating = 506;

            return e;
        }
        static Effect KickbackPenPurple()
        {
            auto e = effect("Kickback Pen (purple)", EffectSlot::KickbackPenPurple);

            e.timeIn60th = 4 * 60;
            e.cooldownIn60th = e.timeIn60th;
            e.bonusStats.penRating = 644;

            return e;
        }
        static Effect KickbackPenBlue()
        {
            auto e = effect("Kickback Pen (blue)", EffectSlot::KickbackPenBlue);

            e.timeIn60th = 4 * 60;
            e.cooldownIn60th = e.timeIn60th;
            e.bonusStats.penRating = 425;

            return e;
        }
        static Effect KickbackCritPowerPurple()
        {
            auto e = effect("Kickback CritPower (purple)", EffectSlot::KickbackCritPowerPurple);

            e.timeIn60th = 4 * 60;
            e.cooldownIn60th = e.timeIn60th;
            e.bonusStats.critPowerRating = 644;

            return e;
        }
        static Effect KickbackCritPowerBlue()
        {
            auto e = effect("Kickback CritPower (blue)", EffectSlot::KickbackCritPowerBlue);

            e.timeIn60th = 4 * 60;
            e.cooldownIn60th = e.timeIn60th;
            e.bonusStats.critPowerRating = 425;

            return e;
        }
    };

private:
    Effects() = delete;
};
