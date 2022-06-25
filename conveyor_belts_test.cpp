#include <gtest/gtest.h>

#include <vector>

#include "conveyor_belts.hpp"

TEST(ConveyorBeltsTest, SortsCoveyorBelts) {
    std::vector<int> H { 1, 3, 2, 2 }, A { 4, 4, 4, 3 }, B { 5, 6, 7, 8 };
    ConveyorBelts belts { H, A, B };
    ASSERT_EQ(belts.belts.size(), 4);
    EXPECT_EQ(belts.belts.at(0), ConveyorBelt({ .h=1, .a=4 }));
    EXPECT_EQ(belts.belts.at(1), ConveyorBelt({ .h=2, .a=3 }));
    EXPECT_EQ(belts.belts.at(2), ConveyorBelt({ .h=2, .a=4 }));
    EXPECT_EQ(belts.belts.at(3), ConveyorBelt({ .h=3, .a=4 }));
}
