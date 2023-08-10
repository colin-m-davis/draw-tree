#include <concepts>
#include <vector>
#include <iostream>
#include <optional>
#include <utility>
#include <random>

// template <typename T>
// concept Ordered = requires(T&& a, T&& b) {
//   { a == b } -> std::convertible_to<bool>;
//   { a < b } -> std::convertible_to<bool>;
// };

template <typename T>
class Tree {
public:
  typedef std::vector<std::pair<T, unsigned>> InputIt;
  struct Node {
    std::optional<T> item = std::nullopt;
    unsigned weight = 0;
    bool activated = false;
    bool hasItem() { return item.has_value(); }
  };

  unsigned numItems;
  std::vector<Node> nodes;
  typedef std::vector<Node>::iterator TreeIt;
  TreeIt root;

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

  unsigned index(TreeIt node) {
    return std::distance(root, node);
  }

  TreeIt getLeftChild(TreeIt parent) {
    return std::next(root, index(parent) * 2 + 1);
  }

  TreeIt getRightChild(TreeIt parent) {
    return std::next(getLeftChild(parent));
  }

  TreeIt getParent(TreeIt child) {
    return std::prev(child, (index(child) - 1) / 2);
  }

  bool isOnLastRow(TreeIt node) {
    return index(node) >= nodes.size() / 2;
  }

  unsigned fillWeights(TreeIt node) {
    if (!isOnLastRow(node)) {
      node->weight = fillWeights(getLeftChild(node)) + fillWeights(getRightChild(node));
    }
    // std::cout << "weight at index " << index(node) << " == " << node->weight << '\n';
    // std::cout << "is on last row? " << isOnLastRow(node) << '\n';
    // std::cout << "L: " << distance(root, getLeftChild(node)) << ", R: " << distance(root, getRightChild(node)) << '\n';
    return node->weight;
  }

  std::mt19937 gen32{std::random_device()()};

  T search(TreeIt node, unsigned roll) {
    std::cout << "search " << index(node) << '\n';
    if (node->hasItem()) {
      return node->item.value();
    } else {
      auto left = getLeftChild(node);
      std::cout << "generated " << roll << " out of " << left->weight << " / " << node->weight << '\n';
      if (roll <= left->weight) {
        return search(left, roll);
      } else {
        return search(getRightChild(node), roll - left->weight);
      }
    }
  }

  void clean(TreeIt node) {

  }

// public:
  Tree(const std::vector<std::pair<T, unsigned>>& vec) :
    numItems(vec.size()),
    nodes(getNumNodes(vec.size()), Node()),
    root(nodes.begin())
  {
    auto lastRowBegin = std::next(root, nodes.size() / 2);
    for (auto inputIt = vec.cbegin(); inputIt < vec.cend(); ++inputIt) {
      *(lastRowBegin++) = Node{std::optional<T>{inputIt->first}, inputIt->second};
    }
    unsigned totalWeight = fillWeights(root);  // TODO: make member
  }

  std::vector<T> get(unsigned count) {
    std::vector<T> result;
    result.reserve(count);
    for (int i = 0; i < count; ++i) {
      std::uniform_int_distribution<unsigned> uni(0, root->weight);
      auto roll = uni(gen32);
      result.push_back(search(root, roll));
    }
    clean(root);
    return result;
  }
};

template<typename T>
void print(std::vector<T> vec) {
  for (const auto& x : vec) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
}


int main() {
  std::vector<std::pair<int, unsigned>> vec{
    {0, 50},
    {1, 1}
  };
  Tree<int> t(vec);
  auto vals = t.get(4);
  print(vals);
  // std::cout << t.search) << '\n';
  return 0;
}
