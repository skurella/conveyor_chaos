#pragma once

#include "conveyor_belts.hpp"
#include "conditional_expected_travel_calculator.hpp"
#include "location_probability_calculator.hpp"

/// The main conveyor chaos solver.
struct ConveyorChaos {
    CondExpTravelCalculator cond_exp_travel;
    LocationProbabilityCalculator distribution;
    ConveyorBelts belts;

    /// Expected travel distance without fixing the direction of any belts.
    double initial_exp_travel() {
        double res = 0;
        for (int i = 0; i < 2 * cond_exp_travel.max_width + 1; i++) {
            res += cond_exp_travel.exp_travel[i] * distribution.distribution[i];
        }
        return res;
    }

    /// Analyzes how fixing each belt could improve the expected travel.
    /// Returns the best improvement found.
    double research_fixing_belts() {
        double best_improvement = 0;
        for (auto belt_it = belts.belts.rbegin(); belt_it != belts.belts.rend(); ++belt_it) {
            double improvement = distribution.improvement(*belt_it);
            if (improvement > best_improvement) { best_improvement = improvement; }
            distribution.process(*belt_it);
        }
        return best_improvement;
    }

    ConveyorChaos(ConveyorBelts&& belts_init) : belts{std::move(belts_init)} {
        cond_exp_travel.process(belts);
    }

    double get_best_result() {
        return initial_exp_travel() - research_fixing_belts();
    }
};
