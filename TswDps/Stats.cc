#include "Stats.hh"
#include "EnemyInfo.hh"

#include <cmath>
#include <iostream>

void Stats::update(const EnemyInfo &enemy)
{
	if (egonPendant)
		finalCritChance = float((65.11f - 120.2f / (exp(critRating / 790.3f) + 1)) / 100 + addedCritChance);
	else 
		finalCritChance = float((55.14f - 100.3f / (exp(critRating / 790.3f) + 1)) / 100 + addedCritChance);

    finalCritPower = float(sqrt(5 * critPowerRating + 625) / 100 + addedCritPower);

	finalPenChance = float(0.312787 - 0.426939 / (exp((penRating - enemy.stats.blockRating) / 388.004) + 1)) + addedPenChance;
	if (finalPenChance < 0)
		finalPenChance = 0.0;

	finalBlockChance = float(0.28291 - 0.362406 / (exp((enemy.stats.blockRating - penRating) / 312.481) + 1)) - addedPenChance;
	if (finalBlockChance < 0)
		finalBlockChance = 0.0;

	finalGlanceChance = float(0.167429 - 0.193239 / (exp((enemy.stats.defenceRating - hitRating) / 150.874) + 1)); // TODO added hit chance
	if (finalGlanceChance < 0)
		finalGlanceChance = 0.0;

	finalEvadeChance = 0; // TODO: hit and evade rating

    finalDmgMultiplier = (1 + additiveDamage) * (1 + multiplicativeDamage);

    // TODO: block, evade, glance?

    finalCombatPower = float(attackRating < 5200 ? (375 - 600 / (exp(attackRating / 1400) + 1)) * (1 + weaponPower / 375)
                                                 : 204.38 + .5471 * weaponPower + (.00008 * weaponPower + .0301) * attackRating);
}

void Stats::dumpDpsGlyphs() const
{
    std::cout << "hit:  " << hitRating << std::endl;
    std::cout << "crit: " << critRating << std::endl;
    std::cout << "cpow: " << critPowerRating << std::endl;
    std::cout << "pen:  " << penRating << std::endl;
}

void Stats::dumpDpsStats(bool updateWithEmpty)
{
    if (updateWithEmpty)
        update(EnemyInfo());
    std::cout << "AR:  " << attackRating << std::endl;
    dumpDpsGlyphs();
    std::cout << "Combat Power:   " << finalCombatPower << std::endl;
    std::cout << "Crit Chance:    " << finalCritChance * 100 << "%" << std::endl;
    std::cout << "Crit Damage:    " << finalCritPower * 100 << "%" << std::endl;
	std::cout << "Pen Chance:     " << finalPenChance * 100 << "%" << std::endl;
	std::cout << "Blocked Chance: " << finalBlockChance * 100 << "%" << std::endl;
	std::cout << "Glanced Chance: " << finalGlanceChance * 100 << "%" << std::endl;
	std::cout << "Evaded Chance:  " << finalEvadeChance * 100 << "%" << std::endl;
    std::cout << "Dmg Multiplier: " << finalDmgMultiplier * 100 << "%" << std::endl;
}
