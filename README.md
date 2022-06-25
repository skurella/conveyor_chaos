# Conveyor chaos

For a problem description, see the [attached document](conveyor_chaos.pdf).

## Mathematical concepts

### Conditional expected travel distance

For a given set of conveyor belts, the conditional expected travel distance is the distance a package is expected to travel along all the belts until is hits the ground when we have prior knowledge that it's initially located at position $x$.

### Location probability

Initially, packages are specified to be distributed uniformly in the $[0, 1e6] \in \mathbb{R}$ inclusive interval.

A belt $B$ influences the probability distribution $P$ of packages as follows:

- $P'(B.a < x < B.b)=0$
  - Probability density directly underneath the belt, excluding its ends, is equal to zero.
- $P'(x \in \{B.a, B.b\}) = P(B.a \leq x \leq B.b)$
  - Sum of probabilities at the $B$'s ends is equal to the total prior probability of a package falling onto $B$.
  - Depending on the belt direction, they can have a $1/0$, $0.5/0.5$, or a $0/1$ ratio.

### Property of linearity - analyzing fixing a belt's direction in isolation

Because probabilities are linear, the expected travel and location distribution can be calculated independently.

Given:
- a belt $B$
- conditional expected travel at its ends $B.a$ and $B.b$
- probability distribution above it $P(x)$

The effect of fixing the belt's direction without considering any other quantities.

## Data structures

TODO

## Algorithms

TODO

### PART 1. Expected horizontal distance travelled (no conveyor belt fixing)

Efficient storage of PDF/CDF that sums of PDF or adds to CDF: segment tree.
Algorithm:
1. Sort conveyor belts by $(y, a)$: $O(N \space log \space N)$
2. Initialize the segment tree for $y_{max}$: $O(W)$
3. For each conveyor belt:
    - accumulate distance travelled: $O(log \space W)$
    - update the segment tree: $O(N \space log \space W)$

### PART 2. Fixing a conveyor belt and obtaining the optimal distance

Observation: fixing a conveyor belt $(y+1, a, b)$ changes
- the distance travelled as well as
- the $CDF(y, a..b)$ or $PDF(y, a | b)$.

Observation: this is a linear system. At any y there exists a linear function $E_y(x)$ representing the expected travel distance conditional on $x$.

Obtaining $E_{y+1}$ from $E_y$ is straightforward:
- when there is no conveyor belt at $x$, then $E_{y+1}(x) = E_y(x)$
- for a random belt from $a$ to $b$, $E_{y+1}(x) = 1/2 * (E_y(a) + E_y(b) + b - a)$
- for a belt fixed right from $a$ to $b$, $E_{y+1}(x) = E_y(b) + b - x$ and vice versa
