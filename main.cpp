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
  using Weight = unsigned long long;
  using Node = unsigned;
  using ItemData = std::vector<std::pair<T, Weight>>;

  unsigned numItems;
  unsigned depth;
  std::vector<Weight> weights;
  ItemData items;
  static const Node root = 1;

  // https://stackoverflow.com/questions/23781506/compile-time-computing-of-number-of-bits-needed-to-encode-n-different-states
  static constexpr unsigned floorLog2(unsigned x) {
    return x == 1 ? 0 : 1 + floorLog2(x >> 1);
  }

  static constexpr unsigned getDepth(unsigned x) {
    return x == 1 ? 0 : floorLog2(x - 1) + 1;
  }

  static constexpr unsigned getNumNodes(unsigned numItems) {
    return (2 << getDepth(numItems));
  }

  constexpr Node getLeftChild(Node parent) {
    return parent * 2;
  }

  constexpr Node getRightChild(Node parent) {
    return (parent * 2) + 1;
  }

  constexpr Node getParent(Node child) {
    return child / 2;
  }

  unsigned fillWeights(Node node) {
    if (node < weights.size() / 2) {
      weights[node] = fillWeights(getLeftChild(node)) + fillWeights(getRightChild(node));
    }
    return weights[node];
  }

  // returns getIndex of result in leafValues
  unsigned search(Node node, unsigned roll) {
    for (int level = 1; level <= depth; ++level) {
      std::cout << "search " << node << '\n';
      auto left = getLeftChild(node);
      auto right = getRightChild(node);
      std::cout << "generated " << roll << " out of " << weights[left] << " / " << weights[node] << '\n';
      if (roll <= weights[left]) {
        node = left;
      } else {
        roll -= weights[left];
        node = right;
      }
      // const auto result = roll <= weights[left] ?
      //   search(left, roll, level + 1) :
      //   search(right, roll - weights[left], level + 1);
      // return result;
    }
    std::cout << "returning from search: " << node - (weights.size() / 2) << '\n';
    return node - (weights.size() / 2);
  }

  auto toggle(unsigned i, bool on = true) -> void {
    const auto leaf = (weights.size() / 2) + i;
    const auto offset = (on ? 1 : -1) * static_cast<int>(items[i].second);
    bubble(leaf, offset);
  }

  void bubble(Node node, int offset) {
    do {
      weights[node] += offset;
      node = getParent(node);
    } while (node > 0);
  }

  std::mt19937 gen32{std::random_device()()};

// public:
  Tree(const ItemData& inputVec) :
    numItems(inputVec.size()),
    items(inputVec),
    depth(getDepth(inputVec.size())),
    weights(getNumNodes(inputVec.size()) + 1, 0)
  {
    std::cout << getNumNodes(2) << ' ' << getNumNodes(3) << ' ' << getNumNodes(4) << '\n';
    auto lastRowIt = weights.size() / 2;
    for (auto inputIt = inputVec.cbegin(); inputIt < inputVec.cend(); ++inputIt) {
      weights[lastRowIt++] = inputIt->second;
    }
    fillWeights(root);
  }

  auto get(unsigned count) -> std::vector<T> {
    std::vector<unsigned> indices;
    indices.reserve(count);
    for (int iterations = 0; iterations < count; ++iterations) {
      std::uniform_int_distribution<Weight> dist(1, weights[root]);
      auto roll = dist(gen32);
      auto i = search(root, roll);
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
  std::vector<std::pair<int, unsigned long long>> inputVec;
  for (int i = 1; i < 5; ++i) {
    inputVec.emplace_back(i, i);
  }
  Tree t(inputVec);
  auto res = t.get(4);
  print(res);
  // std::cout << measure([&t]() { t.get(10); }) << '\n';
  // std::cout << t.search << '\n';
  return 0;
}
