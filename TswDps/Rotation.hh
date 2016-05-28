#pragma once

#include <memory>
#include <functional>
#include <vector>
#include <cassert>

struct Simulation;

struct Rotation
{
    /// a rotation returns the number of active that should be used next
    virtual int nextSkill(int timeIn60th, Simulation const& sim) = 0;

    /// called every time at the start of a sim
    virtual void reset() = 0;

    virtual ~Rotation() {}
};
typedef std::shared_ptr<Rotation> SharedRotation;

struct FixedRotation : Rotation
{
    int currIndex = -1;
    std::vector<int> rotation;

    int nextSkill(int, Simulation const&) override
    {
        assert(currIndex >= 0);
        auto idx = currIndex;
        currIndex = (currIndex + 1) % rotation.size();
        return rotation[idx];
    }

    void reset() override { currIndex = 0; }
    static std::shared_ptr<FixedRotation> create(std::vector<int> const& rot)
    {
        auto r = std::make_shared<FixedRotation>();
        r->rotation = rot;
        return r;
    }
};

/// smart-ish rotation
/// not super optimized
/// tries to consume during buffs
/// tries to build 5
/// skill priority is CD skill > consumer > builder (arbitrated by order)
struct DefaultRotation : Rotation
{
    int maxWaitingForBuffs = 5 * 60; ///< in 60th
	int minResourcesForConsumer = 5; ///< min Nr of resources for consuming

    int nextSkill(int timeIn60th, Simulation const& sim) override;
    void reset() override;

    static std::shared_ptr<DefaultRotation> create()
    {
        return std::make_shared<DefaultRotation>();
    }
};
