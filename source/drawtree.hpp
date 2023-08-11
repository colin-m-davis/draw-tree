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
#include "rules.hpp"

// TODO: convert to random index generator and add wrapper class for T values
class DrawTree {
public:
  using Weight = unsigned long long;
  using Node = unsigned;
  using Index = unsigned;
  using ItemData = std::vector<unsigned>;
  static constexpr bool TOGGLE_ON = true;
  static constexpr bool TOGGLE_OFF = false;

  unsigned numItems;
  unsigned depth;
  std::vector<unsigned> weights;
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

  static constexpr auto getLeftChild(Node parent) -> Node  {
    return parent * 2;
  }

  static constexpr auto getRightChild(Node parent) -> Node {
    return (parent * 2) + 1;
  }

  static constexpr auto getParent(Node child) -> Node {
    return child / 2;
  }

  void fillWeights() {
    for (Node node = firstLeaf - 1; node > 0; --node) {
      nodes[node] = nodes[getLeftChild(node)] + nodes[getRightChild(node)];
    }
  }

  // returns getIndex of result in leafValues
  auto search() -> Index {
    auto node = root;
    std::uniform_int_distribution<Weight> dist(1, nodes[root]);
    auto roll = dist(gen32);
    for (unsigned level = 0; level < depth; ++level) {
      auto left = getLeftChild(node);
      auto right = getRightChild(node);
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

  void toggle(Index leafIndex, bool toggleOn = true) {
    const Node leafNode = firstLeaf + leafIndex;
    const auto offset = (toggleOn ? 1 : -1) * static_cast<long long>(weights[leafIndex]);
    bubble(leafNode, offset);
  }

  void bubble(Node node, int offset) {
    while (node > 0) {
      nodes[node] += offset;
      node = getParent(node);
    }
  }

  std::mt19937 gen32{std::random_device()()};

// public:
  DrawTree(
    const std::vector<unsigned>& weights
    // const std::vector<std::vector<unsigned>>& require = {},
    // const std::vector<std::vector<unsigned>>& cancel = {}
  ) :
    numItems(weights.size()),
    depth(getDepth(weights.size())),
    weights(weights),
    nodes(getNumNodes(weights.size()) + 1, 0),
    firstLeaf(nodes.size() / 2)
  {
    auto leaf = firstLeaf;
    for (auto it = weights.cbegin(); it < weights.cend(); ++it) {
      nodes[leaf++] = *it;
    }
    fillWeights();
  }

  auto get(unsigned count = 1) -> std::vector<Index> {
    std::vector<Index> indices;
    indices.reserve(count);
    for (unsigned iterations = 0; iterations < count; ++iterations) {
      auto i = search() - firstLeaf;
      toggle(i, TOGGLE_OFF);
      indices.push_back(i);
    }
    for (const auto i : indices) {
      toggle(i, TOGGLE_ON);
    }
    return indices; 
  }
};