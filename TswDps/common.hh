#pragma once

#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "jsonxx.hh"

#define SKILL_CNT (8)

#define INF_TIME (3600 * 60)

using namespace std;

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
