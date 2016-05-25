#pragma once

#include <map>
#include <string>

struct SkillTable
{
public:
    static void loadSkillTable(std::string const& filename);

    static float scaling(std::string const& name);

private:
    static std::map<std::string, double> sName2Scaling;

    SkillTable() = delete;
};
