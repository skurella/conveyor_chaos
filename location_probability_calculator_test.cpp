#include <gtest/gtest.h>

#include "conveyor_belt.hpp"
#include "location_probability_calculator.hpp"

TEST(LocationProbabilityCalculatorTest, InitializesToIntervals) {
    LocationProbabilityCalculator c;
    ASSERT_EQ(c.distribution.size(), 2 * c.max_width + 1);
    EXPECT_EQ(c.distribution.at(0), 0);
    EXPECT_EQ(c.distribution.at(1), 1.0 / c.max_width);
    EXPECT_EQ(c.distribution.at(2), 0);
    EXPECT_EQ(c.distribution.at(2 * c.max_width - 1), 1.0 / c.max_width);  
    EXPECT_EQ(c.distribution.at(2 * c.max_width), 0);
    EXPECT_NEAR(c.sum(), 1, 1e-10);
}

TEST(LocationProbabilityCalculatorTest, ProcessesBelt) {
    LocationProbabilityCalculator c;
    ConveyorBelt b { .h = 1, .a = 2, .b = 4 };
    c.process(b);

    double b_prob = static_cast<double>(b.b - b.a) / c.max_width;
    EXPECT_EQ(c.distribution.at(2 * b.a - 1),   1.0 / c.max_width);
    EXPECT_EQ(c.distribution.at(2 * b.a),       b_prob / 2);
    EXPECT_EQ(c.distribution.at(2 * b.a + 1),   0);
    EXPECT_EQ(c.distribution.at(2 * b.b - 1),   0);
    EXPECT_EQ(c.distribution.at(2 * b.b),       b_prob / 2);
    EXPECT_EQ(c.distribution.at(2 * b.b + 1),   1.0 / c.max_width);
}
