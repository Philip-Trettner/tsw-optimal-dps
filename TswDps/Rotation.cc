#include "Rotation.hh"

#include "Simulation.hh"

int DefaultRotation::nextSkill(int timeIn60th, const Simulation& sim)
{
    auto const& skills = sim.skills.skills;

    bool buffsSoon = sim.nextDABS() <= maxWaitingForBuffs;
    bool buffsNow = sim.isEffectActive(EffectSlot::MajorCriticalChance);
    bool efNow = sim.isEffectActive(EffectSlot::ElementalForceBuff);
	bool hasEF = false;
	for (auto const& p : sim.skills.passives)
		if (p.effect == EffectSlot::ElementalForceStacks)
		{
			hasEF = true;
			break;
		}

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
    if (buffsSoon)
        return builder;

    // CD abilities if buffs or EF
	auto useCDs = efNow || buffsNow || !hasEF;
    if (useCDs)
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
				case EffectSlot::GunFu:
					if (sim.resourcesFor(Weapon::Pistol) > 0)
						continue; // don't GF on full resources
				case EffectSlot::LockAndLoad:
					if (sim.resourcesFor(Weapon::Rifle) > 0)
						continue; // don't L&L on full resources
                default:
                    break;
                }
                return i;
            }

    // consumer @ 5
    for (auto i = 0; i < SKILL_CNT; ++i)
        if (!skills[i].name.empty() &&                    //
            skills[i].skilltype == SkillType::Consumer && //
            sim.isSkillReady(i) &&                        //
            sim.resourcesFor(skills[i].weapon) >= minResourcesForConsumer)
            return i;

    // otherwise build
    return builder;
}

void DefaultRotation::reset()
{
}
