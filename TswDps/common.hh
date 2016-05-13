#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <cassert>

#define SKILL_CNT (8)

using namespace std;

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
