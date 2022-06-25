#include <gtest/gtest.h>

#include <vector>

using namespace std;

// At any level y, for a PDF p_y(x) there exist two expected travel distances:
// - status quo, and
// - an optimized one if we can fix one of the belts.

// PDF p_y(x) is:
// - if there is no conveyor belt at (x, y+1), then p_{y+1}(x)
// - if a conveyor belt *interior* exists at (x, y+1), then 0
// - if a conveyor belt *end* exists at (x, y+1), then let s = sum(p_{y+1}(x..x')):
//   - if the belt is fixed, then s or 0
//   - if the belt is random, then s/2

// Reasoning in terms of CDF yields the following invariant: belt at (y+1, a, b) has the following effect on the CDF at y:
// - x < a or x > b: unchanged
// - x > a and x < b: equal to CDF at (y, a)
// - x == a or x == b: +0, +s/2 or +s for s = CDF(y+1, b) - CFG(y+1, a)

// PART 1 - expected horizontal distance travelled (no conveyor belt fixing)
// Efficient storage of PDF/CDF that sums of PDF or adds to CDF: segment tree.
// Algorithm:
// 1. Sort conveyor belts by (y, a): O(N log N)
// 2. Initialize the segment tree for y_max: O(W)
// 3. For each conveyor belt:
//    a) accumulate distance travelled: O(log W)
//    b) update the segment tree: O(N log W)

// PART 2 - fixing a conveyor belt and obtaining the optimal distance
// Observation: fixing a conveyor belt (y+1, a, b) changes
// - the distance travelled as well as
// - the CDF(y, a..b) or PDF(y, a | b).
// Observation: this is a linear system. At any y there exists a linear function E_y(x) representing the expected travel distance conditional on x.
// Obtaining E_{y+1} from E_y is straightforward:
// - when there is no conveyor belt at x, then E_{y+1}(x) = E_y(x)
// - for a random belt from a to b, E_{y+1}(x) = 1/2 * (E_y(a) + E_y(b) + b - a)
// - for a belt fixed right from a to b, E_{y+1}(x) = E_y(b) + b - x
// Question: could we store the possible change in E due to fixing for each conveyor belt? We could then compare it with the PDF when traversing downwards.

// Observation: package must fall strictly within the belt to be moved. This is inconsequential, as integer positions have a probability of 0.

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


TEST(ConveyorBeltTest, BeltsAreOrdered) {
    ConveyorBelt b1 { .h = 1, .a = 3 }, b2 { .h = 1, .a = 4 }, b3 { .h = 2, .a = 3 };
    EXPECT_TRUE(b1 < b2);
    EXPECT_TRUE(b1 < b3);
    EXPECT_TRUE(b2 < b3);
    EXPECT_FALSE(b2 < b1);
    EXPECT_FALSE(b3 < b1);
    EXPECT_FALSE(b3 < b2);
}


/// A sorted vector of ConveyorBelt objects.
struct ConveyorBelts {
    vector<ConveyorBelt> belts;

    ConveyorBelts(const vector<int>& H, const vector<int>& A, const vector<int>& B) {
        belts.reserve(H.size());
        for (auto i = 0; i < H.size(); i++) {
            belts.push_back({.h = H[i], .a = A[i], .b = B[i]});
        }
        sort(belts.begin(), belts.end());
    }
};

TEST(ConveyorBeltsTest, SortsCoveyorBelts) {
    vector<int> H { 1, 3, 2, 2 }, A { 4, 4, 4, 3 }, B { 5, 6, 7, 8 };
    ConveyorBelts belts { H, A, B };
    ASSERT_EQ(belts.belts.size(), 4);
    EXPECT_EQ(belts.belts[0], ConveyorBelt({ .h=1, .a=4 }));
    EXPECT_EQ(belts.belts[1], ConveyorBelt({ .h=2, .a=3 }));
    EXPECT_EQ(belts.belts[2], ConveyorBelt({ .h=2, .a=4 }));
    EXPECT_EQ(belts.belts[3], ConveyorBelt({ .h=3, .a=4 }));
}

// struct SegmentTree {
//     static const int root = 1;
//     static int parent(int i) { return i / 2; }
//     static int left_child(int i) { return i * 2; }
//     static int right_child(int i) { return i * 2 + 1; }
    
//     int n; ///< number of elements in the segment tree, power of 2
//     int zeros_n; ///< number such that `1 << zeros_n == n`
    
//     int el_idx(int i) { return n + i; }
// };

/// Calculates the expected travel distance conditional on x.
struct CondExpTravelCalculator {
    static constexpr int max_width = 1000000;

    /// Expected travel distance at the current height, conditional on x.
    /// Represents discrete integer positions and segments, interleaved, e.g.:
    /// exp_travel[0] corresponds to x = 0
    /// exp_travel[1] corresponds to x > 0 && x < 1
    /// exp_travel[2] corresponds to x = 1, etc.
    vector<double> exp_travel;

    CondExpTravelCalculator() : exp_travel(2 * max_width + 1, 0) {}

    /// Store statistical properties about the state below a given belt.
    void annotate(ConveyorBelt& belt) {
        belt.exp_travel_left = exp_travel[2 * belt.a];
        belt.exp_travel_right = exp_travel[2 * belt.b];
    }

    /// Factor the belt into the current state.
    void advance_past(const ConveyorBelt& belt) {
        // Let i=k represent a segment (k, k+1). If a package falls inside the belt...
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

TEST(CondExpTravelCalculatorTest, InitializesToZero) {
    CondExpTravelCalculator c;
    ASSERT_EQ(c.exp_travel.size(), 2 * c.max_width + 1);
    EXPECT_EQ(c.exp_travel[0], 0);
    EXPECT_EQ(c.exp_travel[2 * c.max_width], 0);
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
    EXPECT_EQ(c.exp_travel[0],          0);
    EXPECT_EQ(c.exp_travel[2 * a - 1],  0);
    EXPECT_EQ(c.exp_travel[2 * a],      0);
    EXPECT_EQ(c.exp_travel[2 * a + 1],  dist);
    EXPECT_EQ(c.exp_travel[2 * b - 1],  dist);
    EXPECT_EQ(c.exp_travel[2 * b],      0);
    EXPECT_EQ(c.exp_travel[2 * b + 1],  0);
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
    EXPECT_EQ(belts.belts[0].exp_travel_left, 0);
    EXPECT_EQ(belts.belts[0].exp_travel_right, 0);
    EXPECT_EQ(belts.belts[1].exp_travel_left, dist1);
    EXPECT_EQ(belts.belts[1].exp_travel_right, 0);
    EXPECT_EQ(belts.belts[2].exp_travel_left, dist1);
    EXPECT_EQ(belts.belts[2].exp_travel_right, dist2);

    // Test exoected travel distance.
    EXPECT_EQ(c.exp_travel[0],          0);
    EXPECT_EQ(c.exp_travel[2 * a1 - 1], 0);
    EXPECT_EQ(c.exp_travel[2 * a1],     0);
    EXPECT_EQ(c.exp_travel[2 * a1 + 1], dist1);
    EXPECT_EQ(c.exp_travel[2 * a3 - 1], dist1);
    EXPECT_EQ(c.exp_travel[2 * a3],     dist1);
    EXPECT_EQ(c.exp_travel[2 * a3 + 1], dist3);
    EXPECT_EQ(c.exp_travel[2 * b3 - 1], dist3);
    EXPECT_EQ(c.exp_travel[2 * b3],     dist2);
    EXPECT_EQ(c.exp_travel[2 * b3 + 1], dist2);
    EXPECT_EQ(c.exp_travel[2 * b2 - 1], dist2);
    EXPECT_EQ(c.exp_travel[2 * b2],     0);
    EXPECT_EQ(c.exp_travel[2 * b2 + 1], 0);
}

struct LocationProbabilityCalculator {
    static constexpr int max_width = 1000000;

    /// Same layout as CondExpTravelCalculator::exp_travel.
    vector<double> distribution;

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
};

TEST(LocationProbabilityCalculatorTest, InitializesToIntervals) {
    LocationProbabilityCalculator c;
    ASSERT_EQ(c.distribution.size(), 2 * c.max_width + 1);
    EXPECT_EQ(c.distribution[0], 0);
    EXPECT_EQ(c.distribution[1], 1.0 / c.max_width);
    EXPECT_EQ(c.distribution[2], 0);
    EXPECT_EQ(c.distribution[2 * c.max_width - 1], 1.0 / c.max_width);  
    EXPECT_EQ(c.distribution[2 * c.max_width], 0);
    EXPECT_NEAR(c.sum(), 1, 1e-10);
}

double exp_travel(const CondExpTravelCalculator& cond_exp_travel, const LocationProbabilityCalculator& distribution) {
    double res = 0;
    for (int i = 0; i < 2 * cond_exp_travel.max_width + 1; i++) {
        res += cond_exp_travel.exp_travel[i] * distribution.distribution[i];
    }
    return res;
}

TEST(ExpTravelTest, ZeroWhenEmpty) {
    CondExpTravelCalculator cond_exp_travel;
    LocationProbabilityCalculator distribution;

    EXPECT_EQ(exp_travel(cond_exp_travel, distribution), 0);
}

TEST(ExpTravelTest, ProcessesBelt) {
    CondExpTravelCalculator cond_exp_travel;
    LocationProbabilityCalculator distribution;
    
    ConveyorBelt belt{.h = 1, .a = 1000, .b = 2000};
    cond_exp_travel.process(belt);

    double belt_total_distance = belt.b - belt.a;
    double belt_expected_distance = belt_total_distance / 2;
    double belt_probability = belt_total_distance / cond_exp_travel.max_width;

    EXPECT_NEAR(exp_travel(cond_exp_travel, distribution), belt_expected_distance * belt_probability, 1e-10);
}
