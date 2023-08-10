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
  using ItemData = std::vector<std::pair<T, unsigned>>;

  unsigned numItems;
  unsigned depth;
  std::vector<Weight> weights;
  ItemData items;
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

  constexpr auto getLeftChild(Node parent) -> Node  {
    return parent * 2;
  }

  constexpr auto getRightChild(Node parent) -> Node {
    return (parent * 2) + 1;
  }

  constexpr auto getParent(Node child) -> Node {
    return child / 2;
  }

  void fillWeights() {
    for (Node node = firstLeaf - 1; node > 0; --node) {
      weights[node] = weights[getLeftChild(node)] + weights[getRightChild(node)];
    }
  }

  // returns getIndex of result in leafValues
  auto search(Node node, unsigned roll) -> unsigned {
    for (int level = 0; level < depth; ++level) {
      auto left = getLeftChild(node);
      auto right = getRightChild(node);
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
    return node;
  }

  void toggle(unsigned leafIndex, bool on = true) {
    const auto leaf = firstLeaf + leafIndex;
    const auto offset = (on ? 1 : -1) * static_cast<long long>(items[leafIndex].second);
    bubble(leaf, offset);
  }

  void bubble(Node node, int offset) {
    while (node > 0) {
      weights[node] += offset;
      node = getParent(node);
    }
  }

  std::mt19937 gen32{std::random_device()()};

// public:
  Tree(const ItemData& inputVec) :
    numItems(inputVec.size()),
    items(inputVec),
    depth(getDepth(inputVec.size())),
    weights(getNumNodes(inputVec.size()) + 1, 0),
    firstLeaf(weights.size() / 2)
  {
    auto leaf = firstLeaf;
    for (auto inputIt = inputVec.cbegin(); inputIt < inputVec.cend(); ++inputIt) {
      weights[leaf++] = inputIt->second;
    }
    fillWeights();
  }

  auto get(unsigned count) -> std::vector<T> {
    std::vector<unsigned> indices;
    indices.reserve(count);
    for (int iterations = 0; iterations < count; ++iterations) {
      std::uniform_int_distribution<Weight> dist(1, weights[root]);
      auto roll = dist(gen32);
      auto i = search(root, roll) - firstLeaf;
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
    // std::cout << x << ' ';
  }
  // std::cout << '\n';
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

template <typename T>
bool validate(std::vector<T> vec) {
  std::unordered_set<T> items(vec.begin(), vec.end());
  return vec.size() == items.size();
}

bool test(int n) {
  std::cout << "testing " << n << '\n';
  std::vector<std::pair<int, unsigned>> inputVec;
  for (int i = 1; i <= n; ++i) {
    inputVec.emplace_back(i, i);
  }
  Tree t(inputVec);
  auto res = t.get(n);
  return validate(res);
}

int main() {
  for (int i = 1; i < 2050; ++i) {
    if (!test(i)) {
      std::cout << i << '\n';
      return 1;
    }
  }
  return 0;
}
