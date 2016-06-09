#include <gtest/gtest.h>

#include "Simulation.hh"
#include "CombatLog.hh"
#include "Builds.hh"

TEST(Variance, RandomBuild)
{
    std::default_random_engine random(testing::GTEST_FLAG(random_seed));

    for (int i = 0; i < 10; ++i)
    {
        Simulation s;
        s.loadBuild(Builds::random(random));
        s.init();

        s.lowVarianceMode = false;
        s.simulate(timestr(".1h"));
        auto dpsHigh = s.totalDPS();

        s.resetStats();
        s.lowVarianceMode = true;
        s.simulate(timestr(".1h"));
        auto dpsLow = s.totalDPS();

        auto maxError = fmax(dpsHigh, dpsLow) * 0.1; // 10% max error (for 10min tests)

        ASSERT_NEAR(dpsHigh, dpsLow, maxError);
    }
}
