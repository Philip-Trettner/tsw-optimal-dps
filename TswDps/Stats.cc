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
}

void Stats::dumpDpsGlyphs() const
{
    std::cout << "hit:  " << hitRating << std::endl;
    std::cout << "crit: " << critRating << std::endl;
    std::cout << "cpow: " << critPowerRating << std::endl;
    std::cout << "pen:  " << penRating << std::endl;
}
