#include <gtest/gtest.h>

#include "conveyor_chaos.hpp"

TEST(ConveyorChaosTest, ZeroWhenEmpty) {
    ConveyorChaos c({{}, {}, {}});

    EXPECT_EQ(c.initial_exp_travel(), 0);
}

TEST(ConveyorChaosTest, ProcessesBelt) {
    ConveyorChaos c({{1}, {1000}, {2000}});

    auto& belt = c.belts.belts.at(0);
    double belt_total_distance = belt.b - belt.a;
    double belt_expected_distance = belt_total_distance / 2;
    double belt_probability = belt_total_distance / c.cond_exp_travel.max_width;

    EXPECT_NEAR(c.initial_exp_travel(), belt_expected_distance * belt_probability, 1e-10);
}

TEST(ConveyorChaosTest, SampleTestCase1) {
    ConveyorChaos c({{10, 20}, {100000, 400000}, {600000, 800000}});
    EXPECT_NEAR(c.get_best_result(), 155000.0, 10e-6);
}

TEST(ConveyorChaosTest, SampleTestCase2) {
    ConveyorChaos c({{2, 8, 5, 9, 4}, {5000, 2000, 7000, 9000, 0}, {7000, 8000, 11000, 11000, 4000}});
    EXPECT_NEAR(c.get_best_result(), 36.5, 10e-6);
}
