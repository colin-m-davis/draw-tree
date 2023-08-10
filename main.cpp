#include <vector>
#include <iostream>
#include <utility>
#include <random>
#include <iterator>
#include <algorithm>
#include <chrono>
#include <functional>
#include "rules.cpp"

// template <typename T>
// concept Ordered = requires(T&& a, T&& b) {
//   { a == b } -> std::convertible_to<bool>;
//   { a < b } -> std::convertible_to<bool>;
// };

template <typename T>
class Tree {
public:
  struct Node {
    unsigned weight = 0;
  };

  using ItemData = std::vector<std::pair<T, unsigned>>;
  using TreeIt = unsigned;

  unsigned numItems;
  unsigned depth;
  std::vector<Node> nodes;
  ItemData items;
  TreeIt root = 0;

  // https://stackoverflow.com/questions/23781506/compile-time-computing-of-number-of-bits-needed-to-encode-n-different-states
  static constexpr unsigned floorLog2(unsigned x) {
    return x == 1 ? 0 : 1 + floorLog2(x >> 1);
  }

  static constexpr unsigned getDepth(unsigned x) {
    return x == 1 ? 0 : floorLog2(x - 1) + 1;
  }

  static constexpr unsigned getNumNodes(unsigned numItems) {
    return (2 << getDepth(numItems)) - 1;
  }

  TreeIt getLeftChild(TreeIt parent) {
    return (parent * 2) + 1;
  }

  TreeIt getRightChild(TreeIt parent) {
    return (parent * 2) + 2;
  }

  TreeIt getParent(TreeIt child) {
    return (child - 1) / 2;
  }

  unsigned fillWeights(TreeIt node) {
    if (node < nodes.size() / 2) {
      nodes[node].weight = fillWeights(getLeftChild(node)) + fillWeights(getRightChild(node));
    }
    return nodes[node].weight;
  }

  // returns getIndex of result in leafValues
  unsigned search(TreeIt node, unsigned roll, int level) {
    std::cout << "search " << node << '\n';
    if (level == depth) {
      return node - (nodes.size() / 2);
    } else {
      auto left = getLeftChild(node);
      auto right = getRightChild(node);
      std::cout << "generated " << roll << " out of " << nodes[left].weight << " / " << nodes[node].weight << '\n';
      const auto result = roll <= nodes[left].weight ?
        search(left, roll, level + 1) :
        search(right, roll - nodes[left].weight, level + 1);
      return result;
    }
  }

  auto toggle(unsigned i, bool on = true) -> void {
    const auto leaf = (nodes.size() / 2) + i;
    const auto offset = (on ? 1 : -1) * static_cast<int>(items[i].second);
    bubble(leaf, offset);
  }

  void bubble(TreeIt node, int offset) {
    nodes[node].weight += offset;
    if (node > 0) {
      bubble(getParent(node), offset);
    }
  }

// public:
  Tree(const ItemData& inputVec) :
    numItems(inputVec.size()),
    items(inputVec),
    depth(getDepth(inputVec.size())),
    nodes(getNumNodes(inputVec.size()), Node())
  {
    auto lastRowIt = nodes.size() / 2;
    for (auto inputIt = inputVec.cbegin(); inputIt < inputVec.cend(); ++inputIt) {
      nodes[lastRowIt++].weight = inputIt->second;
    }
    fillWeights(root);
  }

  std::mt19937 gen32{std::random_device()()};

  auto get(unsigned count) -> std::vector<T> {
    std::vector<unsigned> indices;
    indices.reserve(count);
    for (int iterations = 0; iterations < count; ++iterations) {
      std::uniform_int_distribution<unsigned> dist(1, nodes[root].weight);
      auto roll = dist(gen32);
      std::cout << "getting next value ! ! !" << '\n';
      auto i = search(root, roll, 0);
      std::cout << "got " << i << "(" << items[i].first << ")" << '\n';
      toggle(i, false);
      indices.push_back(i);
    }
    std::vector<T> result;
    result.reserve(count);
    for (const auto i : indices) {
      toggle(i, true);
      result.push_back(items[i].first);
    }
    return result;
  }
};

template<typename T>
void print(std::vector<T> inputVec) {
  for (const auto& x : inputVec) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
}

uint64_t timeSinceEpochMillisec() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}


uint64_t measure(const std::function<void()>& f) {
  auto start = timeSinceEpochMillisec();
  f();
  auto end = timeSinceEpochMillisec();
  return end - start;
}

int main() {
  std::vector<std::pair<int, unsigned>> inputVec;
  for (int i = 1; i < 11; ++i) {
    inputVec.emplace_back(i, i);
  }
  Tree t(inputVec);
  auto res = t.get(10);
  print(res);
  // std::cout << measure([&t]() { t.get(1000); }) << '\n';
  // std::cout << t.search << '\n';
  return 0;
}
