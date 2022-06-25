#pragma once

#include <vector>

/// Calculates the probability mass function for each x.
/// Starts at top height and goes down.
struct LocationProbabilityCalculator {
    static constexpr int max_width = 1000000;

    /// Same layout as CondExpTravelCalculator::exp_travel.
    std::vector<double> distribution;

    LocationProbabilityCalculator() : distribution(2 * max_width + 1, 0) {
        for (int i = 0; i < max_width; i++) {
            distribution[2 * i + 1] = 1.0 / max_width;
        }
    }

    double sum() const {
        double sum = 0.0;
        for (auto d : distribution) { sum += d; }
        return sum;
    }

    /// Probability of hitting a specific conveyor belt.
    double probability(const ConveyorBelt& belt) const {
        double total_belt_probability = 0;
        for (auto i = 2 * belt.a + 1; i <= 2 * belt.b - 1; ++i) {
            total_belt_probability += distribution[i];
        }
        return total_belt_probability;
    }

    double improvement(const ConveyorBelt& belt) const {
        /// Improvement in expected travel below the belt due to fixing it left.
        /// Fixing it in the opposite direction negates this improvement.
        double improvement_below_left = probability(belt) * (belt.exp_travel_left - belt.exp_travel_right) / 2;
        double improvement_below_right = -improvement_below_left;

        /// Improvement in travel along the belt.
        double exp_travel_left = 0, exp_travel_right = 0;
        for (auto i = 2 * belt.a + 1; i <= 2 * belt.b - 1; ++i) {
            exp_travel_left += distribution[i] * static_cast<double>(i - 2 * belt.a) / 2;
            exp_travel_right += distribution[i] * static_cast<double>(2 * belt.b - i) / 2;
        }
        double exp_travel_unfixed = probability(belt) * static_cast<double>(belt.b - belt.a) / 2;

        double improvement_left = improvement_below_left + exp_travel_left - exp_travel_unfixed;
        double improvement_right = improvement_below_right + exp_travel_right - exp_travel_unfixed;
        return std::max(improvement_left, improvement_right);
    }

    void process(const ConveyorBelt& belt) {
        double total_belt_probability = probability(belt);
        distribution[2 * belt.a] += total_belt_probability / 2;
        distribution[2 * belt.b] += total_belt_probability / 2;
        for (auto i = 2 * belt.a + 1; i <= 2 * belt.b - 1; ++i) { distribution[i] = 0; }
    }
};
