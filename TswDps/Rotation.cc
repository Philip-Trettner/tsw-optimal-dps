#include "Rotation.hh"

#include "Simulation.hh"

int DefaultRotation::nextSkill(int timeIn60th, const Simulation& sim)
{
	auto const& skills = sim.skills.skills;

	// buff status
	bool majorBuffsSoon = sim.nextDABS() <= maxWaitingForMajorBuffs;
	bool majorBuffsNow = sim.isEffectActive(EffectSlot::MajorCriticalChance);
	bool efNow = sim.isEffectActive(EffectSlot::ElementalForceBuff);
	bool ffNow = sim.isEffectActive(EffectSlot::FatalFlourishBuff);
	bool wcNow = sim.isEffectActive(EffectSlot::MothersWrathBuff);
	bool bloodOffering = sim.isEffectActive(EffectSlot::BloodOffering);
	bool hasEF = false;
	bool hasWC = false;
	bool hasFF = false;
	for (auto const& p : sim.skills.passives)
	{
		if (p.effect == EffectSlot::ElementalForceStacks)
			hasEF = true;
		if (p.effect == EffectSlot::FatalFlourishStacks)
			hasFF = true;
	}
	if (sim.gear.pieces[Gear::MajorMid].signet.passive.effect == EffectSlot::MothersWrathStacks)
		hasWC = true;
	bool hasBuffState = false;
	if (hasEF && considerBuffEF)
		hasBuffState = true;
	if (hasFF && considerBuffFF)
		hasBuffState = true;
	if (hasWC && considerBuffWC)
		hasBuffState = true;

	int builder = -1;
	for (int i = 0; i < SKILL_CNT; ++i)
		if (skills[i].skilltype == SkillType::Builder)
		{
			builder = i;
			break;
		}
	assert(builder >= 0 && "no builder found");

	// use bombardment off CD
	for (int i = 0; i < SKILL_CNT; ++i)
		if (skills[i].name == "Bombardment" && sim.isSkillReady(i))
			return i;

	// only build if buffs soon
	if (majorBuffsSoon)
		return builder;

	// CD abilities if buffs or EF
	auto buffedNow = false;
	if (majorBuffsNow)
		buffedNow = true;
	if (efNow && considerBuffEF)
		buffedNow = true;
	if (ffNow && considerBuffFF)
		buffedNow = true;
	if (wcNow && considerBuffWC)
		buffedNow = true;
	// TODO: doom
	if (buffedNow || !hasBuffState)
		for (auto i = 0; i < SKILL_CNT; ++i)
			if (!skills[i].name.empty() &&                    //
				skills[i].skilltype != SkillType::Consumer && //
				skills[i].skilltype != SkillType::Builder &&  //
				sim.isSkillReady(i))
			{
				// special skills
				switch (skills[i].passive.effect)
				{
				case EffectSlot::FullMomentum:
					if (sim.resourcesFor(Weapon::Hammer) > 0)
						continue; // don't FM on full resources
					break;
				case EffectSlot::GunFu:
					if (sim.resourcesFor(Weapon::Pistol) > 0)
						continue; // don't GF on full resources
					break;
				case EffectSlot::LockAndLoad:
					if (sim.resourcesFor(Weapon::Rifle) > 0)
						continue; // don't L&L on full resources
					break;
				default:
					break;
				}
				return i;
			}

	// consumer @ 5 (or custom)
	for (auto i = 0; i < SKILL_CNT; ++i)
		if (!skills[i].name.empty() &&                    //
			skills[i].skilltype == SkillType::Consumer && //
			sim.isSkillReady(i))
        {
            bool canUse = true;

			// fixed resource consumer only work if blood (without active offering) or enough res
            if (skills[i].fixedConsumerResources > 0)
            {
                if (sim.resourcesFor(skills[i].weapon) < skills[i].fixedConsumerResources)
                    if (bloodOffering || skills[i].weapon != Weapon::Blood)
                        canUse = false;
            }
            else if (sim.resourcesFor(skills[i].weapon) == 0)
                canUse = false;

            // cannot use skill currently
            if (!canUse)
                continue;

			// use consumers only on X res (or, if tryToConsumeOnBuffed and buffs, immediately)
			if (!(tryToConsumeOnBuffed && buffedNow))
			{
				if (skills[i].weapon == sim.gear.leftWeapon &&
					sim.resourcesFor(skills[i].weapon) < minResourcesForLeftConsumer)
					continue;

				if (skills[i].weapon == sim.gear.rightWeapon &&
					sim.resourcesFor(skills[i].weapon) < minResourcesForRightConsumer)
					continue;
			}

			return i;
		}

    // otherwise build
    return builder;
}

void DefaultRotation::reset()
{
}
