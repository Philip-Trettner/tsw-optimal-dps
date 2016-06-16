#pragma once

#include "common.hh"
#include "Simulation.hh"
#include  "Optimizer.hh"

struct Scenario
{
    string name;

    int fightTimeIn60th;
    int totalTimeIn60th;

    void loadFromJson(jsonxx::Object const& o, std::string const& defName)
    {
        using namespace jsonxx;
        name = o.get<String>("Name", defName);
        fightTimeIn60th = ticksFromJsonObj(o, "Fight Time");
        totalTimeIn60th = ticksFromJsonObj(o, "Total Time");
    }

    jsonxx::Object saveToJson() const
    {
        using namespace jsonxx;
        Object o;
        o << "Name" << name;
        o << "Fight Time" << toTimeStr(fightTimeIn60th);
        o << "Total Time" << toTimeStr(totalTimeIn60th);
        return o;
    }

    void apply(Simulation& s, Optimizer& o) const
    {
        o.timePerFight = fightTimeIn60th;
        
    }
};
