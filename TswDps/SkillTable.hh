#pragma once

#include <map>
#include <string>

struct SkillTable
{
public:
    static void loadSkillTable(std::string const& filename);
    static void loadVDMTable(std::string const& filename);

    static float scaling(std::string const& name);

private:
    static std::map<std::string, double> sName2Scaling;
    static std::map<std::string, std::string> sName2ID;

    SkillTable() = delete;
};
