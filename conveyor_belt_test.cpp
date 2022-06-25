#include <gtest/gtest.h>

#include "conveyor_belt.hpp"

TEST(ConveyorBeltTest, BeltsAreOrdered) {
    ConveyorBelt b1 { .h = 1, .a = 3 }, b2 { .h = 1, .a = 4 }, b3 { .h = 2, .a = 3 };
    EXPECT_TRUE(b1 < b2);
    EXPECT_TRUE(b1 < b3);
    EXPECT_TRUE(b2 < b3);
    EXPECT_FALSE(b2 < b1);
    EXPECT_FALSE(b3 < b1);
    EXPECT_FALSE(b3 < b2);
}
