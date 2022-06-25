#pragma once

#include <vector>

#include "conveyor_belt.hpp"
#include "conveyor_belts.hpp"

/// Calculates the expected travel distance conditional on x.
/// Starts at ground and goes up.
struct CondExpTravelCalculator {
    static constexpr int max_width = 1000000;

    /// Expected travel distance at the current height, conditional on x.
    /// Represents discrete integer positions and segments, interleaved, e.g.:
    /// exp_travel[0] corresponds to x = 0
    /// exp_travel[1] corresponds to x > 0 && x < 1
    /// exp_travel[2] corresponds to x = 1, etc.
    std::vector<double> exp_travel;

    CondExpTravelCalculator() : exp_travel(2 * max_width + 1, 0) {}

    /// Store statistical properties about the state below a given belt.
    void annotate(ConveyorBelt& belt) {
        belt.exp_travel_left = exp_travel[2 * belt.a];
        belt.exp_travel_right = exp_travel[2 * belt.b];
    }

    /// Factor the belt into the current state.
    void advance_past(const ConveyorBelt& belt) {
        // If a package falls inside the belt...
        for (auto i = 2 * belt.a + 1; i <= 2 * belt.b - 1; i++) {
            // ...it's expected to travel half the belt's length and land on one of the sides.
            exp_travel[i] = static_cast<double>(belt.b - belt.a) / 2 + (exp_travel[2 * belt.a] + exp_travel[2 * belt.b]) / 2;
        }
    }

    void process(ConveyorBelt& belt) {
        annotate(belt);
        advance_past(belt);
    }

    /// Annotate and advance past all the belts in order.
    void process(ConveyorBelts& belts) {
        // Belts are sorted by height.
        for (auto& belt : belts.belts) process(belt);
    }
};
