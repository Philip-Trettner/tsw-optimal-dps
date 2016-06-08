#pragma once

#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>

#include "jsonxx.hh"

#define SKILL_CNT (8)

#define INF_TIME (1000 * 3600 * 60)

using namespace std;

inline string toTimeStr(int ticksIn60th)
{
    if (ticksIn60th % (60 * 60 * 60) == 0)
        return to_string(ticksIn60th / (60 * 60 * 60)) + "h";
    if (ticksIn60th % (60 * 60) == 0)
        return to_string(ticksIn60th / (60 * 60)) + "m";
    if (ticksIn60th % (60) == 0)
        return to_string(ticksIn60th / (60)) + "s";

    return to_string(ticksIn60th) + "t";
}

inline int ticksFromTimeStr(string const& s)
{
    if (s.empty())
        return -1; // empty

    auto unit = s[s.size() - 1];
    auto nrs = isdigit(unit) ? s : s.substr(0, s.size() - 1);
    istringstream iss(nrs);
    double val;
    iss >> val;

    switch (unit)
    {
    case 'h':
        val *= 60 * 60 * 60;
        break;
    case 'm':
        val *= 60 * 60;
        break;
    case 's':
    default: // default is seconds
        val *= 60;
        break;
    case 't':
        break;
    }

    return val;
}

inline int ticksFromJsonObj(jsonxx::Object const& o, string name, int def = -1)
{
    using namespace jsonxx;
    if (o.has<String>(name))
        return ticksFromTimeStr(o.get<String>(name));
    else if (o.has<Number>(name))
        return o.get<Number>(name) * 60; // seconds
    else
        return def; // invalid
}

template <typename T>
inline void hash_combine(size_t& seed, T const& v)
{
    seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

inline void printTable(vector<string> const& header, vector<vector<float>> const& vals, ostream& oss = std::cout, char delim = ';')
{
    auto first = true;
    for (auto const& s : header)
    {
        if (first)
            first = false;
        else
            oss << delim;

        oss << s;
    }
    oss << endl;

    for (auto const& v : vals)
    {
        first = true;
        for (auto const& s : v)
        {
            if (first)
                first = false;
            else
                oss << delim;

            oss << s;
        }
        oss << endl;
    }
}

/// Returns the path of the given file without trailing /
inline std::string pathOf(std::string const& filename)
{
    auto minPosA = filename.rfind('/');
    if (minPosA == std::string::npos)
        minPosA = 0;

    auto minPosB = filename.rfind('\\');
    if (minPosB == std::string::npos)
        minPosB = 0;

    auto minPos = minPosA < minPosB ? minPosB : minPosA;
    if (minPos == 0)
        return "";
    return filename.substr(0, minPos);
}
