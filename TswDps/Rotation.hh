#pragma once

#include <memory>
#include <functional>
#include <vector>

struct Simulation;

struct Rotation
{
    /// a rotation returns the number of active that should be used next
    virtual int nextSkill(int timeIn60th, Simulation const& sim) = 0;

    /// called every time at the start of a sim
    virtual void reset() = 0;
};
typedef std::shared_ptr<Rotation> SharedRotation;

struct FixedRotation : Rotation
{
    int currIndex;
    std::vector<int> rotation;

    int nextSkill(int timeIn60th, Simulation const& sim) override
    {
        auto idx = currIndex;
        currIndex = (currIndex + 1) % rotation.size();
        return rotation[idx];
    }

    void reset() override { currIndex = 0; }
};
