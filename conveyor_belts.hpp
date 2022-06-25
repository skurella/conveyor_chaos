#pragma once

#include <vector>

#include "conveyor_belt.hpp"

/// A sorted std::vector of ConveyorBelt objects.
struct ConveyorBelts {
    std::vector<ConveyorBelt> belts;

    ConveyorBelts(const std::vector<int>& H, const std::vector<int>& A, const std::vector<int>& B) {
        belts.reserve(H.size());
        for (auto i = 0; i < H.size(); i++) {
            belts.push_back({.h = H[i], .a = A[i], .b = B[i]});
        }
        std::sort(belts.begin(), belts.end());
    }
};
