#pragma once

#include "Skill.hh"

/**
 * @brief Skill library
 *
 * For scalings see https://docs.google.com/spreadsheets/d/1z9b23xHPNQuqmZ5t51SeIMq2rlI6d8mPyWp9BmGNxjc/
 */
struct Skills
{
public:
private:
    template <Weapon weapon, DmgType dmgtype>
    struct Base
    {
    protected:
        Base() = delete;

        static Skill skill(std::string const& name, SkillType skilltype)
        {
            Skill s;
            s.name = name;
            s.weapon = weapon;
            s.dmgtype = dmgtype;
            s.skilltype = skilltype;
            return s;
        }
    };

    static float scaling(std::string const& name);

public:
    struct Pistol : private Base<Weapon::Pistol, DmgType::Ranged>
    {
        static Skill TheBusiness()
        {
            auto s = skill("The Business", SkillType::Builder);
            s.timeIn60th = 60;
            s.hits = 3;
            s.dmgScaling = scaling(s.name);
            return s;
        }

        static Skill GunFu()
        {
            auto s = skill("Gun-Fu", SkillType::None);
            s.timeIn60th = 60;
            s.passive.trigger = Trigger::FinishActivation;
            s.passive.effect = EffectSlot::GunFu;
            s.cooldownIn60th = 30 * 60;
            s.reduceWeaponConsumerCD = 4 * 60;
            s.slotForDmgAug = false;
            return s;
        }

        static Skill Collaboration()
        {
            auto s = skill("Collaboration", SkillType::Builder);
            s.timeIn60th = 60;
            s.casttimeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            return s;
        }

        static Skill HairTrigger()
        {
            auto s = skill("Hair Trigger", SkillType::Builder);
            s.timeIn60th = 60;
            s.hits = 4;
            s.subtype = SubType::Focus;
            s.dmgScaling = scaling(s.name);
            s.channeling = true;
            return s;
        }

        static Skill Shootout()
        {
            auto s = skill("Shootout", SkillType::Consumer);
            s.timeIn60th = 150;
            s.cooldownIn60th = 60 * 4;
            s.hits = 5;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            s.channeling = true;
            s.subtype = SubType::Focus;
            return s;
        }

        static Skill Marked()
        {
            auto s = skill("Marked", SkillType::None);
            s.timeIn60th = 90;
            s.casttimeIn60th = 90;
            s.cooldownIn60th = 60 * 30;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            return s;
        }

        static Skill StartAndFinish()
        {
            auto s = skill("Start & Finish", SkillType::Consumer);
            s.timeIn60th = 60;
            s.cooldownIn60th = 60 * 4;
            s.hits = 2;
            s.dmgScalingA = scaling(s.name + " 1st");
            s.specialHitsA = 1;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            s.channeling = true;
            return s;
        }

        static Skill BondStrongBond()
        {
            auto s = skill("Bond, Strong Bond", SkillType::Consumer);
            s.timeIn60th = 3 * 60;
            s.hits = 10;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            s.cooldownIn60th = 60 * 4;
            s.channeling = true;
            return s;
        }

        static Skill Big45()
        {
            auto s = skill("Big 45", SkillType::Consumer);
            s.casttimeIn60th = 90; // TODO: CD passive
            s.timeIn60th = 90;     // TODO: CD passive
            s.cooldownIn60th = 60 * 4;
            s.hits = 1;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            return s;
        }

        static Skill DirtyTricks()
        {
            auto s = skill("Dirty Tricks", SkillType::Elite);
            s.cooldownIn60th = 25 * 60;
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            s.animaDeviation = true;
            return s;
        }

        static Skill GunCrazy()
        {
            auto s = skill("Gun Crazy", SkillType::Elite);
            s.cooldownIn60th = 20 * 60;
            s.timeIn60th = 3 * 60;
            s.hits = 10;
            s.channeling = true;
            s.dmgScalingA = scaling(s.name + " 3xA");
            s.dmgScalingB = scaling(s.name + " 3xB");
            s.dmgScalingC = scaling(s.name + " 3xC");
            s.dmgScaling = scaling(s.name + " Final");
            s.specialHitsA = 3;
            s.specialHitsB = 3;
            s.specialHitsC = 3;
            s.appliesVulnerability = DmgType::Magic;
            return s;
        }
    };

    struct Shotgun : private Base<Weapon::Shotgun, DmgType::Ranged>
    {
        static Skill Striker()
        {
            auto s = skill("Striker", SkillType::Builder);
            s.timeIn60th = 60;
            s.hits = 1;
            s.subtype = SubType::Strike;
            s.dmgScaling = scaling(s.name);
            return s;
        }

        static Skill SingleBarrel()
        {
            auto s = skill("Single Barrel", SkillType::Builder);
            s.timeIn60th = 60;
            s.casttimeIn60th = 30;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            return s;
        }

        static Skill OutForAKill()
        {
            auto s = skill("Out for a Kill", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            s.cooldownIn60th = 60 * 4;
            return s;
        }

        static Skill SureShot()
        {
            auto s = skill("Sure Shot", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            s.cooldownIn60th = 60 * 4;
            return s;
        }

        static Skill Takedown()
        {
            auto s = skill("Takedown", SkillType::None);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            s.cooldownIn60th = 60 * 25;
            return s;
        }

        /// Assumes close range
        static Skill RagingBullet()
        {
            auto s = skill("Raging Bullet", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 1;
            s.subtype = SubType::Strike;
            s.dmgScaling = scaling(s.name + " A @1");
            s.dmgScaling5 = scaling(s.name + " A @5");
            s.cooldownIn60th = 60 * 4;
            return s;
        }

        static Skill PointBlank()
        {
            auto s = skill("Point Blank", SkillType::Elite);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            s.cooldownIn60th = 60 * 25;

            s.animaDeviation = true;
            s.appliesVulnerability = DmgType::Melee;
            return s;
        }

        static Skill Kneecapper()
        {
            auto s = skill("Kneecapper", SkillType::Elite);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            s.cooldownIn60th = 60 * 25;

            s.animaDeviation = true;
            return s;
        }

        static Skill Bombardment()
        {
            auto s = skill("Bombardment", SkillType::Elite);
            s.timeIn60th = 60;
            s.cooldownIn60th = 60 * 30;
            s.passive.trigger = Trigger::StartActivation;
            s.passive.effect = EffectSlot::Bombardment;
            s.passive.effectStacks = 8;
            return s;
        }

        static Skill LockStockBarrel()
        {
            auto s = skill("Lock, Stock & Barrel", SkillType::None);
            s.cooldownIn60th = 30 * 60;
            s.passive.effect = EffectSlot::LockStockBarrel;
            s.passive.trigger = Trigger::FinishActivation;
            s.slotForDmgAug = false; // TODO: CHECK ME
            return s;
        }

        static Skill ShotgunWedding()
        {
            auto s = skill("Shotgun Wedding", SkillType::Elite);
            s.timeIn60th = 2 * 60 + 30;
            s.hits = 5;
            s.dmgScaling = scaling(s.name);
            s.cooldownIn60th = 20 * 60;
            s.channeling = true;
            s.baseDmgIncPerHit = 0.25f; // 25% more dmg per hit

            s.appliesVulnerability = DmgType::Magic;

            return s;
        }
    };

    struct Rifle : private Base<Weapon::Rifle, DmgType::Ranged>
    {
        static Skill SafetyOff()
        {
            auto s = skill("Safety Off", SkillType::Builder);
            s.timeIn60th = 60;
            s.casttimeIn60th = 60;
            s.hits = 3;
            s.subtype = SubType::Burst;
            s.dmgScaling = scaling(s.name);
            return s;
        }

        static Skill LockAndLoad()
        {
            auto s = skill("Lock & Load", SkillType::None);
            s.passive.trigger = Trigger::FinishActivation;
            s.passive.effect = EffectSlot::LockAndLoad;
            s.cooldownIn60th = 25 * 60;
            s.reduceWeaponConsumerCD = 4 * 60;
            s.slotForDmgAug = false;
            return s;
        }


        static Skill TriggerHappy()
        {
            auto s = skill("Trigger Happy", SkillType::Builder);
            s.timeIn60th = 60;
            s.casttimeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            return s;
        }

        static Skill Shellshocker()
        {
            auto s = skill("Shellshocker", SkillType::Elite);
            s.timeIn60th = 2 * 60;
            s.hits = 8;
            s.dmgScaling = scaling(s.name);
            s.cooldownIn60th = 25 * 60;
            s.channeling = true;

            s.animaDeviation = true;
            s.appliesVulnerability = DmgType::Melee;

            return s;
        }

        static Skill RedMist()
        {
            auto s = skill("Red Mist", SkillType::Elite);
            s.timeIn60th = 2 * 60;
            s.casttimeIn60th = s.timeIn60th;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            s.cooldownIn60th = 20 * 60;

            s.appliesVulnerability = DmgType::Magic;

            return s;
        }

        static Skill ThreeRoundBurst()
        {
            auto s = skill("Three Round Burst", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 3;
            s.cooldownIn60th = 4 * 60;
            s.subtype = SubType::Burst;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            return s;
        }
    };

    struct Chaos : private Base<Weapon::Chaos, DmgType::Magic>
    {
        static Skill RunRampant()
        {
            auto s = skill("Run Rampant", SkillType::Builder);
            s.timeIn60th = 60;
            s.hits = 3;
            s.subtype = SubType::Burst;
            s.dmgScaling = scaling(s.name);
            return s;
        }

        static Skill HandOfChange()
        {
            auto s = skill("Hand of Change", SkillType::Builder);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            return s;
        }

        static Skill FourHorsemen()
        {
            auto s = skill("Four Horsemen", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 4;
            s.subtype = SubType::Burst;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            return s;
        }

        static Skill PullingTheStrings()
        {
            auto s = skill("Pulling the Strings", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            // TODO: minor hit chance
            return s;
        }

        static Skill SufferingAndSolace()
        {
            auto s = skill("Suffering and Solace", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            return s;
        }

        static Skill ChaoticPull()
        {
            auto s = skill("Chaotic Pull", SkillType::None);
            s.timeIn60th = 60;
            s.cooldownIn60th = 35 * 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            return s;
        }

        static Skill Schism()
        {
            auto s = skill("Schism", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            s.dmgScalingLow = scaling(s.name + " @1 low");
            s.dmgScaling5Low = scaling(s.name + " @5 low");
            return s;
        }

        static Skill CallForEris()
        {
            auto s = skill("Call for Eris", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 1;
            s.extraHitPerResource = 1;
            s.fixedMultiHitPenalty = 0.75; // ?????? no fucking idea
            s.subtype = SubType::Burst;
            s.specialHitsA = 1;
            s.dmgScalingA = scaling(s.name + " 1st");
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            return s;
        }

        static Skill AmorFati()
        {
            auto s = skill("Amor Fati", SkillType::None);
            s.cooldownIn60th = 60 * 60;
            s.passive.trigger = Trigger::FinishActivation;
            s.passive.effect = EffectSlot::AmorFati;
            s.slotForDmgAug = false; // TODO: CHECK ME
            return s;
        }

        static Skill DominoEffect()
        {
            auto s = skill("Domino Effect", SkillType::Elite);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            s.cooldownIn60th = 25 * 60;
            return s;
        }

        static Skill PrisonerOfFate()
        {
            auto s = skill("Prisoner of Fate", SkillType::Elite);
            s.timeIn60th = 5 * 60;
            s.hits = 5;
            s.dmgScaling = scaling(s.name);
            s.dmgScalingLow = scaling(s.name + " low");
            s.cooldownIn60th = 20 * 60;
            s.channeling = true;
            s.animaDeviation = true;
            s.appliesVulnerability = DmgType::Melee;
            return s;
        }

        static Skill EyeOfPandemonium()
        {
            auto s = skill("Eye of Pandemonium", SkillType::Elite);
            s.timeIn60th = 60;
            s.cooldownIn60th = 25 * 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            s.passive.trigger = Trigger::FinishActivation;
            s.passive.effect = EffectSlot::EyeOfPandemonium;
            s.passive.effectStacks = 10;
            s.animaDeviation = true;
            return s;
        }

        static Skill GravitationalAnomaly()
        {
            auto s = skill("Gravitational Anomaly", SkillType::Elite);
            s.timeIn60th = 3 * 60;
            s.hits = 3;
            s.dmgScaling = scaling(s.name);
            s.cooldownIn60th = 25 * 60;
            s.channeling = true;
            return s;
        }

        static Skill Paradox()
        {
            auto s = skill("Paradox", SkillType::None);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            s.cooldownIn60th = 10 * 60;
            return s;
        }
    };

    struct Blood : private Base<Weapon::Blood, DmgType::Magic>
    {
        static Skill BoilingBlood()
        {
            auto s = skill("Boiling Blood", SkillType::Builder);
            s.timeIn60th = 60;
            s.casttimeIn60th = s.timeIn60th;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            return s;
        }
        static Skill CardiacArrest()
        {
            auto s = skill("Cardiac Arrest", SkillType::Elite);
            s.timeIn60th = 60;
            s.casttimeIn60th = s.timeIn60th;
            s.cooldownIn60th = 25 * 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            s.animaDeviation = true;
            s.appliesVulnerability = DmgType::Melee;
            return s;
        }
        static Skill Exsanguinate()
        {
            auto s = skill("Exsanguinate", SkillType::Consumer);
            s.timeIn60th = 150;
            s.cooldownIn60th = 60 * 4;
            s.hits = 5;
            s.fixedConsumerResources = 5;
            s.dmgScaling = scaling(s.name);
            s.channeling = true;
            s.subtype = SubType::Focus;
            return s;
        }
        static Skill Bloodline()
        {
            auto s = skill("Bloodline", SkillType::Builder);
            s.timeIn60th = 60;
            s.hits = 4;
            s.subtype = SubType::Focus;
            s.dmgScaling = scaling(s.name);
            s.channeling = true;
            return s;
        }
        static Skill Bloodshot()
        {
            auto s = skill("Bloodshot", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 1;
            s.fixedConsumerResources = 2;
            s.dmgScaling = scaling(s.name);
            return s;
        }
    };

    struct Elemental : private Base<Weapon::Elemental, DmgType::Magic>
    {
        static Skill Shock()
        {
            auto s = skill("Shock", SkillType::Builder);
            s.timeIn60th = 60;
            s.casttimeIn60th = s.timeIn60th;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            return s;
        }
        static Skill ElectricalBolt()
        {
            auto s = skill("Electrical Bolt", SkillType::Builder);
            s.timeIn60th = 60;
            s.casttimeIn60th = s.timeIn60th;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            // TODO: 2 res if hindered
            return s;
        }
        static Skill Ignition()
        {
            auto s = skill("Ignition", SkillType::Builder);
            s.timeIn60th = 60;
            s.casttimeIn60th = s.timeIn60th;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            s.subtype = SubType::Strike;
            return s;
        }

        static Skill Combust()
        {
            auto s = skill("Combust", SkillType::Consumer);
            s.timeIn60th = 60 + 30; // TODO: passive
            s.casttimeIn60th = s.timeIn60th;
            s.hits = 1;
            s.fixedConsumerResources = 2; // TODO: 1 if hindered
            s.dmgScaling = scaling(s.name);
            return s;
        }

        static Skill FlameStrike()
        {
            auto s = skill("Flame Strike", SkillType::Consumer);
            s.timeIn60th = 60;
            s.casttimeIn60th = s.timeIn60th;
            s.hits = 1;
            s.fixedConsumerResources = 2;
            s.dmgScaling = scaling(s.name);
            s.subtype = SubType::Strike;
            return s;
        }

        static Skill Blaze()
        {
            auto s = skill("Blaze", SkillType::Consumer);
            s.timeIn60th = 60 + 30;
            s.casttimeIn60th = s.timeIn60th;
            s.hits = 1;
            s.fixedConsumerResources = 3;
            s.dmgScaling = scaling(s.name);
            // TODO: Aidolon Passive
            return s;
        }

        static Skill MoltenEarth()
        {
            auto s = skill("Molten Earth", SkillType::Elite);
            s.timeIn60th = 60;
            s.cooldownIn60th = 25 * 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            s.animaDeviation = true;
            return s;
        }

        static Skill ThorsHammer()
        {
            auto s = skill("Thor's Hammer", SkillType::Consumer);
            s.timeIn60th = 2 * 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            s.fixedConsumerResources = 5;
            s.subtype = SubType::Strike;
            return s;
        }

        static Skill HardReset()
        {
            auto s = skill("Hard Reset", SkillType::Elite);
            s.timeIn60th = 2 * 60;
            s.cooldownIn60th = 35 * 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            s.passive.bonusStats.addedCritChance = 1; // guaranteed crit
            s.animaDeviation = true;
            return s;
        }

        static Skill Overload()
        {
            auto s = skill("Overload", SkillType::Elite);
            s.timeIn60th = 3 * 60;
            s.cooldownIn60th = 20 * 60;
            s.hits = 3;
            s.channeling = true;
            s.dmgScalingA = scaling(s.name);
            s.specialHitsA = 2;
            s.dmgScaling = scaling(s.name + " final");
            s.appliesVulnerability = DmgType::Melee;
            return s;
        }

        static Skill Whiteout()
        {
            auto s = skill("Whiteout", SkillType::Elite);
            s.timeIn60th = 60;
            s.casttimeIn60th = s.timeIn60th;
            s.cooldownIn60th = 25 * 60;
            s.passive.trigger = Trigger::FinishActivation;
            s.passive.effect = EffectSlot::Whiteout;
            s.passive.effectStacks = 16;
            s.animaDeviation = true;
            return s;
        }

        static Skill PowerLine()
        {
            auto s = skill("Power Line", SkillType::Elite);
            s.timeIn60th = 60;
            s.cooldownIn60th = 20 * 60;
            s.passive.trigger = Trigger::StartActivation;
            s.passive.effect = EffectSlot::PowerLine;
            s.passive.effectStacks = 10;
            s.appliesVulnerability = DmgType::Ranged;
            return s;
        }

        static Skill FireManifestation()
        {
            auto s = skill("Fire Manifestation", SkillType::Consumer);
            s.timeIn60th = 60;
            s.cooldownIn60th = 10 * 60;
            s.fixedConsumerResources = 2;
            s.passive.trigger = Trigger::StartActivation;
            s.passive.effect = EffectSlot::FireManifestation;
            s.passive.effectStacks = 4;
            return s;
        }

        static Skill LightningManifestation()
        {
            auto s = skill("Lightning Manifestation", SkillType::Consumer);
            s.timeIn60th = 60;
            s.cooldownIn60th = 15 * 60;
            s.fixedConsumerResources = 2;
            s.passive.trigger = Trigger::StartActivation;
            s.passive.effect = EffectSlot::LightningManifestation;
            s.passive.effectStacks = 10;
            return s;
        }
    };

    struct Blade : private Base<Weapon::Blade, DmgType::Melee>
    {
        static Skill DelicateStrike()
        {
            auto s = skill("Delicate Strike", SkillType::Builder);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            return s;
        }
        static Skill GrassCutter()
        {
            auto s = skill("Grass Cutter", SkillType::Builder);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            return s;
        }
        static Skill SteelEcho()
        {
            auto s = skill("Steel Echo", SkillType::None);
            s.cooldownIn60th = 60 * 60;
            s.passive.trigger = Trigger::StartActivation;
            s.passive.effect = EffectSlot::SteelEcho;
            return s;
        }
        static Skill SlingBlade()
        {
            auto s = skill("Sling Blade", SkillType::None);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            s.cooldownIn60th = 35 * 60;
            // TODO: afflict
            return s;
        }
        static Skill BalancedBlade()
        {
            auto s = skill("Balanced Blade", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            return s;
        }
        static Skill BindingWounds()
        {
            auto s = skill("Binding Wounds", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            return s;
        }
        static Skill DancingBlade()
        {
            auto s = skill("Dancing Blade", SkillType::Consumer);
            s.timeIn60th = 150;
            s.hits = 5;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            s.channeling = true;
            s.subtype = SubType::Focus;
            return s;
        }
        static Skill StunningSwirl()
        {
            auto s = skill("Stunning Swirl", SkillType::Elite);
            s.timeIn60th = 60;
            s.cooldownIn60th = 20 * 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            s.appliesVulnerability = DmgType::Ranged;
            return s;
        }
        static Skill FourSeasons()
        {
            auto s = skill("Four Seasons", SkillType::Elite);
            s.timeIn60th = 150;
            s.hits = 5;
            s.cooldownIn60th = 20 * 60;
            s.passive.bonusStats.addedPenChance = 1.0; // guaranteed pen
            s.dmgScalingA = scaling(s.name);
            s.specialHitsA = 4;
            s.dmgScaling = scaling(s.name + " Final");
            s.channeling = true;
            s.appliesVulnerability = DmgType::Magic;
            return s;
        }
    };

    struct Hammer : private Base<Weapon::Hammer, DmgType::Melee>
    {
        static Skill Smash()
        {
            auto s = skill("Smash", SkillType::Builder);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            return s;
        }
        static Skill Haymaker()
        {
            auto s = skill("Haymaker", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            s.subtype = SubType::Strike;
            return s;
        }
        static Skill StonesThrow()
        {
            auto s = skill("Stone's Throw", SkillType::None);
            s.timeIn60th = 60;
            s.hits = 1;
            s.cooldownIn60th = 15 * 60;
            s.dmgScaling = scaling(s.name);
            return s;
        }
        static Skill Shockwave()
        {
            auto s = skill("Shockwave", SkillType::Elite);
            s.timeIn60th = 60;
            s.cooldownIn60th = 25 * 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            s.appliesVulnerability = DmgType::Magic;
            return s;
        }
        static Skill Eruption()
        {
            auto s = skill("Eruption", SkillType::Elite);
            s.timeIn60th = 60;
            s.cooldownIn60th = 25 * 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            s.appliesVulnerability = DmgType::Ranged;
            return s;
        }

        static Skill MoltenSteel()
        {
            auto s = skill("Molten Steel", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            s.passive.bonusStats.addedCritChance = 30 / 100.f;
            s.passive.bonusStats.addedCritPower = 15 / 100.f;

            return s;
        }

        static Skill BoneBreaker()
        {
            auto s = skill("Bone Breaker", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");

            return s;
        }

        static Skill FullMomentum()
        {
            auto s = skill("Full Momentum", SkillType::None);
            s.cooldownIn60th = 30 * 60;
            s.passive.effect = EffectSlot::FullMomentum;
            s.passive.trigger = Trigger::FinishActivation;
            s.slotForDmgAug = false; // TODO: CHECK ME
            return s;
        }
    };

    struct Fist : private Base<Weapon::Fist, DmgType::Melee>
    {
        static Skill SeeRed()
        {
            auto s = skill("See Red", SkillType::Elite);
            s.timeIn60th = 4 * 60;
            s.hits = 20;
            s.dmgScaling = scaling(s.name);
            s.cooldownIn60th = 25 * 60;
            s.channeling = true;
            s.animaDeviation = true;
            s.appliesVulnerability = DmgType::Magic;
            return s;
        }

        static Skill Reckless()
        {
            auto s = skill("Reckless", SkillType::None);
            s.cooldownIn60th = 40 * 60;
            s.passive.trigger = Trigger::FinishActivation;
            s.passive.effect = EffectSlot::Reckless;
            s.slotForDmgAug = false; // TODO: CHECK ME
            return s;
        }

        static Skill Claw()
        {
            auto s = skill("Claw", SkillType::Builder);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            return s;
        }

        static Skill PreyOnTheWeak()
        {
            auto s = skill("Prey on the Weak", SkillType::Builder);
            s.timeIn60th = 60;
            s.hits = 3;
            s.dmgScaling = scaling(s.name);
            s.subtype = SubType::Burst;
            return s;
        }

        static Skill WildAtHeart()
        {
            auto s = skill("Wild at Heart", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            return s;
        }

        static Skill TearEmUp()
        {
            auto s = skill("Tear Em Up", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            s.passive.trigger = Trigger::Hit;
            s.passive.effect = EffectSlot::TearEmUp;
            return s;
        }

        static Skill GoForTheThroat()
        {
            auto s = skill("Go for the Throat", SkillType::Elite);
            s.timeIn60th = 60;
            s.cooldownIn60th = 25 * 60;
            s.hits = 1;
            s.dmgScaling = scaling(s.name);
            s.passive.trigger = Trigger::FinishActivation;
            s.passive.effect = EffectSlot::GoForTheThroat;
            s.passive.effectStacks = 10;
            s.animaDeviation = true;
            return s;
        }

        static Skill OneTwo()
        {
            auto s = skill("One-Two", SkillType::Consumer);
            s.timeIn60th = 60;
            s.hits = 2;
            s.dmgScalingA = scaling(s.name + " 1st");
            s.specialHitsA = 1;
            s.dmgScaling = scaling(s.name + " @1");
            s.dmgScaling5 = scaling(s.name + " @5");
            s.subtype = SubType::Burst;
            return s;
        }
    };

    struct Chainsaw : private Base<Weapon::Aux, DmgType::None>
    {
        static Skill Timber()
        {
            auto s = skill("Timber", SkillType::None);
            s.timeIn60th = 60;
            s.hits = 1;
            s.cooldownIn60th = 15 * 60;
            s.dmgScaling = scaling(s.name);
            s.chanceForScaleInc = 0.33f;
            s.scaleIncPerc = .45f;
            s.slotForDmgAug = false;
            return s;
        }
    };

    static Skill empty() { return Skill(); }
    static std::vector<Skill> all();

private:
    Skills() = delete;
};
