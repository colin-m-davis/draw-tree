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
    return std::prev(child, (std::distance(root, child) - 1) / 2);
  }

  bool isOnLastRow(TreeIt node) {
    return std::distance(root, node) >= nodes.size() / 2;
  }

  unsigned fillWeights(TreeIt node) {
    if (!isOnLastRow(node)) {
      node->weight = fillWeights(getLeftChild(node)) + fillWeights(getRightChild(node));
    }
    // std::cout << "weight at index " << std::distance(root, node) << " == " << node->weight << '\n';
    // std::cout << "is on last row? " << isOnLastRow(node) << '\n';
    // std::cout << "L: " << distance(root, getLeftChild(node)) << ", R: " << distance(root, getRightChild(node)) << '\n';
    return node->weight;
  }

  std::mt19937 gen32{std::random_device()()};

  T search(TreeIt node) {
    std::cout << "search " << index(node) << '\n';
    if (node->hasItem()) {
      return node->item.value();
    } else {
      std::uniform_int_distribution<unsigned> uni(0, node->weight);
      auto gen = uni(gen32);
      auto left = getLeftChild(node);
      std::cout << "generated " << gen << " out of " << left->weight << " / " << node->weight << '\n';
      if (gen <= left->weight) {
        return search(left);
      }
      auto right = getRightChild(node);
      return search(right);
    }
  }

// public:
  Tree(const std::vector<std::pair<int, unsigned>>& vec) :
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
};

int main() {
  std::vector<std::pair<int, unsigned>> vec{
    {0, 0},
  };
  Tree<int> t(vec);
  std::cout << t.search(t.root) << '\n';
  return 0;
}
