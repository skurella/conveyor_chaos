#pragma once

#include <tuple>

struct ConveyorBelt {
    // coordinates
    int h;
    int a;
    int b;

    // annotations
    double exp_travel_left; ///< expected conditional travel distance at (h - 1, a)
    double exp_travel_right; ///< expected conditional travel distance at (h - 1, b)
    
    // Conveyor belts do not overlap, therefore it's enough to compare their left ends.
    bool operator<(const ConveyorBelt& other) const { return std::tie(h, a) < std::tie(other.h, other.a); }
    bool operator==(const ConveyorBelt& other) const { return std::tie(h, a) == std::tie(other.h, other.a); }
};
