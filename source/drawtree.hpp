#include <vector>
#include <iostream>
#include <utility>
#include <random>
#include <iterator>
#include <algorithm>
#include <chrono>
#include <functional>
#include <unordered_set>
#include <memory>
#include <cassert>
#include "rules.hpp"

// TODO: convert to random index generator and add wrapper class for T values
class DrawTree {
public:
  using Weight = uint_fast64_t;
  using Index = uint_fast32_t;
  using Node = uint_fast32_t;
  static constexpr bool TOGGLE_ON = true;
  static constexpr bool TOGGLE_OFF = false;

  unsigned numItems;
  std::vector<Weight> weights;
  Index depth;
  std::vector<Weight> nodes;
  static const Node root = 1;
  const Node firstLeaf;

  // https://stackoverflow.com/questions/23781506/compile-time-computing-of-number-of-bits-needed-to-encode-n-different-states
  static constexpr auto floorLog2(unsigned x) -> unsigned {
    return x == 1 ? 0 : 1 + floorLog2(x >> 1);
  }

  static constexpr auto getDepth(unsigned x) -> unsigned {
    return x == 1 ? 0 : floorLog2(x - 1) + 1;
  }

  static constexpr auto getNumNodes(unsigned numItems) -> Node {
    return 2 << getDepth(numItems);
  }

  static constexpr auto getLeftChild(Node parent) -> Node {
    return parent * 2;
  }

  static constexpr auto getRightChild(Node parent) -> Node {
    return (parent * 2) + 1;
  }

  static constexpr auto getParent(Node child) -> Node {
    return child / 2;
  }

  void fillWeights() {
    for (Node node = firstLeaf - 1; node >= root; --node) {
      nodes[node] = nodes[getLeftChild(node)] + nodes[getRightChild(node)];
    }
  }

  // returns getIndex of result in leafValues
  auto search(Node node, Weight roll) -> Node {
    for (Index level = 0; level < depth; ++level) {
      auto left = getLeftChild(node);
      auto right = getRightChild(node);
      assert(nodes[left] + nodes[right] == nodes[node]);
      if (roll <= nodes[left]) {
        node = left;
      } else {
        roll -= nodes[left];
        node = right;
      }
      // const auto result = roll <= nodes[left] ?
      //   search(left, roll, level + 1) :
      //   search(right, roll - nodes[left], level + 1);
      // return result;
    }
    return node;
  }

  template <bool on = TOGGLE_ON>
  void toggle(Index leafIndex) {
    const Node leaf = firstLeaf + leafIndex;
    bubble<on>(leaf, weights[leafIndex]);
  }

  template <bool add = true>
  void bubble(Node node, Weight offset) {
    while (node) {
      if constexpr (add) {
        nodes[node] += offset;
      } else {
        nodes[node] -= offset;
      }
      node = getParent(node);
    }
  }

  std::mt19937_64 gen64{std::random_device()()};

// public:
  DrawTree(auto&& w) :
    numItems(w.size()),
    weights(std::forward<decltype(w)>(w)),
    depth(getDepth(w.size())),
    nodes(getNumNodes(w.size()) + 1, 0),
    firstLeaf(nodes.size() / 2)
  {
    auto leaf = firstLeaf;
    for (auto inputIt = w.cbegin(); inputIt < w.cend(); ++inputIt) {
      nodes[leaf++] = *inputIt;
    }
    fillWeights();
  }

  auto get(unsigned count) -> std::vector<unsigned> {
    std::vector<unsigned> indices;
    indices.reserve(count);
    for (unsigned iterations = 0; iterations < count; ++iterations) {
      std::uniform_int_distribution<Weight> dist(1, nodes[root]);
      auto roll = dist(gen64);
      auto i = search(root, roll) - firstLeaf;
      toggle<TOGGLE_OFF>(i);
      indices.push_back(i);
    }
    for (const auto i : indices) {
      toggle<TOGGLE_ON>(i);
    }
    return indices; 
  }
};