At any level y, for a PDF p_y(x) there exist two expected travel distances:
- status quo, and
- an optimized one if we can fix one of the belts.

PDF p_y(x) is:
- if there is no conveyor belt at (x, y+1), then p_{y+1}(x)
- if a conveyor belt *interior* exists at (x, y+1), then 0
- if a conveyor belt *end* exists at (x, y+1), then let s = sum(p_{y+1}(x..x')):
  - if the belt is fixed, then s or 0
  - if the belt is random, then s/2

Reasoning in terms of CDF yields the following invariant: belt at (y+1, a, b) has the following effect on the CDF at y:
- x < a or x > b: unchanged
- x > a and x < b: equal to CDF at (y, a)
- x == a or x == b: +0, +s/2 or +s for s = CDF(y+1, b) - CFG(y+1, a)

PART 1 - expected horizontal distance travelled (no conveyor belt fixing)
Efficient storage of PDF/CDF that sums of PDF or adds to CDF: segment tree.
Algorithm:
1. Sort conveyor belts by (y, a): O(N log N)
2. Initialize the segment tree for y_max: O(W)
3. For each conveyor belt:
   a) accumulate distance travelled: O(log W)
   b) update the segment tree: O(N log W)

PART 2 - fixing a conveyor belt and obtaining the optimal distance
Observation: fixing a conveyor belt (y+1, a, b) changes
- the distance travelled as well as
- the CDF(y, a..b) or PDF(y, a | b).
Observation: this is a linear system. At any y there exists a linear function E_y(x) representing the expected travel distance conditional on x.
Obtaining E_{y+1} from E_y is straightforward:
- when there is no conveyor belt at x, then E_{y+1}(x) = E_y(x)
- for a random belt from a to b, E_{y+1}(x) = 1/2 * (E_y(a) + E_y(b) + b - a)
- for a belt fixed right from a to b, E_{y+1}(x) = E_y(b) + b - x
Question: could we store the possible change in E due to fixing for each conveyor belt? We could then compare it with the PDF when traversing downwards.

Observation: package must fall strictly within the belt to be moved. This is inconsequential, as integer positions have a probability of 0.

```c++
struct SegmentTree {
    static const int root = 1;
    static int parent(int i) { return i / 2; }
    static int left_child(int i) { return i * 2; }
    static int right_child(int i) { return i * 2 + 1; }
    
    int n; ///< number of elements in the segment tree, power of 2
    int zeros_n; ///< number such that `1 << zeros_n == n`
    
    int el_idx(int i) { return n + i; }
};
```
