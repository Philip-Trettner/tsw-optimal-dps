#include "Stats.hh"
#include "EnemyInfo.hh"

#include <cmath>
#include <iostream>

void Stats::update(const EnemyInfo &enemy)
{
    (void)enemy;

    finalCritChance = float((55.14f - 100.3f / (exp(critRating / 790.3f) + 1)) / 100 + addedCritChance);
    finalCritPower = float(sqrt(5 * critPowerRating + 625) / 100 + addedCritPower);
    finalPenChance = .3f + addedPenChance; // TODO: make this dependent on enemy stats
    finalDmgMultiplier = (1 + additiveDamage) * (1 + multiplicativeDamage);

    // TODO: block, evade, glance?

    finalCombatPower = 1000; // TODO: calc me!
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
    std::cout << "Combat Power: " << finalCombatPower << std::endl;
    std::cout << "Crit Chance: " << finalCritChance * 100 << "%" << std::endl;
    std::cout << "Crit Damage: " << finalCritPower * 100 << "%" << std::endl;
    std::cout << "Pen Chance:  " << finalPenChance * 100 << "%" << std::endl;
    std::cout << "Dmg Multiplier: " << finalDmgMultiplier * 100 << "%" << std::endl;
}
