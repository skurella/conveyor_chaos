#include <gtest/gtest.h>

#include "conditional_expected_travel_calculator.hpp"

TEST(CondExpTravelCalculatorTest, InitializesToZero) {
    CondExpTravelCalculator c;
    ASSERT_EQ(c.exp_travel.size(), 2 * c.max_width + 1);
    EXPECT_EQ(c.exp_travel.at(0), 0);
    EXPECT_EQ(c.exp_travel.at(2 * c.max_width), 0);
}

TEST(CondExpTravelCalculatorTest, ProcessesBelt) {
    CondExpTravelCalculator c;
    int h = 1, a = 2, b = 3;
    ConveyorBelt belt { .h = h, .a = a, .b = b };
    c.process(belt);

    // Annotations should be zero, because no travel is expected underneath the belt.
    EXPECT_EQ(belt.exp_travel_left, 0);
    EXPECT_EQ(belt.exp_travel_right, 0);

    // We expect 5/2 units of travel on average above the belt and outside of it.
    auto dist = static_cast<double>(b - a) / 2;
    EXPECT_EQ(c.exp_travel.at(0),          0);
    EXPECT_EQ(c.exp_travel.at(2 * a - 1),  0);
    EXPECT_EQ(c.exp_travel.at(2 * a),      0);
    EXPECT_EQ(c.exp_travel.at(2 * a + 1),  dist);
    EXPECT_EQ(c.exp_travel.at(2 * b - 1),  dist);
    EXPECT_EQ(c.exp_travel.at(2 * b),      0);
    EXPECT_EQ(c.exp_travel.at(2 * b + 1),  0);
}

TEST(CondExpTravelCalculatorTest, ProcessesBelts) {
    CondExpTravelCalculator c;
    int a1 = 2, b1 = 5;
    int a2 = 3, b2 = 5;
    int a3 = 3, b3 = 4;
    ConveyorBelts belts({1, 2, 10}, {a1, a2, a3}, {b1, b2, b3});
    //     1 2 3 4 5 6
    // 10:     |-|
    // ...
    // 2:      |---|
    // 1:    |-----|
    c.process(belts);

    double dist1 = static_cast<double>(b1 - a1) / 2;
    double dist2 = static_cast<double>(b2 - a2) / 2 + dist1 / 2;
    double dist3 = static_cast<double>(b3 - a3) / 2 + dist1 / 2 + dist2 / 2;

    // Test annotations.    
    EXPECT_EQ(belts.belts.at(0).exp_travel_left, 0);
    EXPECT_EQ(belts.belts.at(0).exp_travel_right, 0);
    EXPECT_EQ(belts.belts.at(1).exp_travel_left, dist1);
    EXPECT_EQ(belts.belts.at(1).exp_travel_right, 0);
    EXPECT_EQ(belts.belts.at(2).exp_travel_left, dist1);
    EXPECT_EQ(belts.belts.at(2).exp_travel_right, dist2);

    // Test exoected travel distance.
    EXPECT_EQ(c.exp_travel.at(0),          0);
    EXPECT_EQ(c.exp_travel.at(2 * a1 - 1), 0);
    EXPECT_EQ(c.exp_travel.at(2 * a1),     0);
    EXPECT_EQ(c.exp_travel.at(2 * a1 + 1), dist1);
    EXPECT_EQ(c.exp_travel.at(2 * a3 - 1), dist1);
    EXPECT_EQ(c.exp_travel.at(2 * a3),     dist1);
    EXPECT_EQ(c.exp_travel.at(2 * a3 + 1), dist3);
    EXPECT_EQ(c.exp_travel.at(2 * b3 - 1), dist3);
    EXPECT_EQ(c.exp_travel.at(2 * b3),     dist2);
    EXPECT_EQ(c.exp_travel.at(2 * b3 + 1), dist2);
    EXPECT_EQ(c.exp_travel.at(2 * b2 - 1), dist2);
    EXPECT_EQ(c.exp_travel.at(2 * b2),     0);
    EXPECT_EQ(c.exp_travel.at(2 * b2 + 1), 0);
}
