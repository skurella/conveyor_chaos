# Conveyor chaos

[![CMake](https://github.com/skurella/conveyor_chaos/actions/workflows/test.yml/badge.svg)](https://github.com/skurella/conveyor_chaos/actions/workflows/test.yml)

For the problem description, see the [attached document](conveyor_chaos.pdf).

## Mathematical concepts

### Conditional expected travel distance

For a given set of conveyor belts $S$ spanning from the ground to height $y$, the conditional expected travel distance $E_y(x)$ is the distance a package is expected to travel along all the belts until is hits the ground when we have prior knowledge that it's initially located at position $x$.

The same quantity $E_{y+1}$ for $S \cup S'$, where $S'$ are the conveyor belts at height $y+1$, can be calculated from $E_y$ as follows:
- if $\exists_B: B.a < x < B.b$, then:
  - for a random belt, $E_{y+1}(x) \triangleq \frac{1}{2} (E_y(B.a) + E_y(B.b) + B.b - B.a)$
  - for a belt directed right, $E_{y+1}(x) \triangleq E_y(B.b) + B.b - x$
- otherwise, $E_{y+1}(x) \triangleq E_y(x)$


### Location probability distribution

Initially, packages are specified to be distributed uniformly in the $[0, 10^6] \in \mathbb{R}$ inclusive interval.

A belt $B$ influences the probability distribution $P$ of packages as follows:

- $P'(B.a < x < B.b)=0$
  - Probability density directly underneath the belt, excluding its ends, is equal to zero.
- $P'(x \in \{B.a, B.b\}) = P(B.a \leq x \leq B.b)$
  - Sum of probabilities at the $B$'s ends is equal to the total prior probability of a package falling onto $B$.
  - Depending on the belt direction, they can have a $1-0$, $\frac{1}{2}-\frac{1}{2}$, or a $0-1$ ratio.

### Property of linearity - analyzing fixing a belt's direction in isolation

Because probabilities are linear, the expected travel and location distribution can be calculated independently.

Specifically, the effect of fixing a belt $B$'s direction can be calculated from:
- conditional expected travel at its ends $B.a$ and $B.b$
- probability distribution above it $P(x)$

 without considering any other quantities.

## Data structures

TODO:
- describe probability distribution encoding for mixed discrete / continuous densities
- replace vector with a "RLE vector" that supports constant updates to segments in logarithmic time

## Algorithms

### Sorting conveyor belts

Conveyor belts need to be processed in:
- ascending order for the conditional expected travel distance, and
- descending order for the location probability distribution.

Sorting the conveyor belts has time complexity of $O(N \space log \space N)$.

### Conditional expected travel distance (without fixing belt directions)

Let us calculate the expected travel distance $E_{y_{max}}(x)$ should a package be dropped at position $(x, y_{max})$.

Algorithm:
1. Initialize the expected travel $E$ &mdash; $O(W)$
3. For each of $N$ conveyor belts:
    1. Annotate the conveyor belt $B$ with current values of $E(B.a)$ and $E(B.b)$ &mdash; $O(N)$
    2. Calculate expected travel for packages falling onto the belt &mdash; $O(N)$
    3. Update $E(B.a < x < B.b)$ &mdash; $O(N^2)$, assuming average belt length proportional to $N$

### Obtaining the optimal distance

Now that $E_{y_{max}}$ is known, the total expected travel distance *without* fixing any conveyor belts is equal to:

$$
E_{init} \triangleq
\int_0^{10^6} E_{y_{max}}(x) \cdot P_{y_{max}}(x) \space dx =
\frac{1}{10^6} \int_0^{10^6} E_{y_{max}}(x) \space dx
$$

### Location probability distribution (with fixing conveyor belt directions)

TODO
