#include "Skills.hh"

#include "SkillTable.hh"

float Skills::scaling(const string &name)
{
    return SkillTable::scaling(name);
}
