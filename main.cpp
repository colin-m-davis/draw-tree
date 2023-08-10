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
    unsigned weight = 0;
    bool activated = false;
  };

  unsigned numItems;
  unsigned depth;
  std::vector<Node> nodes;
  std::vector<T> leafValues{};
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

  unsigned search(TreeIt node, unsigned roll, int iterations) {
    std::cout << "search " << index(node) << '\n';
    if (iterations == depth) {
      return index(node) - nodes.size() / 2;
    } else {
      auto left = getLeftChild(node);
      std::cout << "generated " << roll << " out of " << left->weight << " / " << node->weight << '\n';
      if (roll <= left->weight) {
        return search(left, roll, iterations + 1);
      } else {
        return search(getRightChild(node), roll - left->weight, iterations + 1);
      }
    }
  }

  void clean(TreeIt node) {
    if (!node->activated) {
      node->activated = true;
      if (!isOnLastRow(node)) {
        clean(getLeftChild(node));
        clean(getRightChild(node));
      }
    }
  }

// public:
  Tree(const std::vector<std::pair<T, unsigned>>& inputVec) :
    numItems(inputVec.size()),
    depth(getDepth(inputVec.size())),
    nodes(getNumNodes(inputVec.size()), Node()),
    root(nodes.begin())
  {
    leafValues.reserve(inputVec.size());
    auto lastRowIt = std::next(root, nodes.size() / 2);
    for (auto inputIt = inputVec.cbegin(); inputIt < inputVec.cend(); ++inputIt) {
      *(lastRowIt++) = Node{inputIt->second, true};
      leafValues.push_back(inputIt->first);
    }
    for (; lastRowIt < nodes.end(); ++lastRowIt) {
      *lastRowIt = Node{};
    }
    unsigned totalWeight = fillWeights(root);  // TODO: make member
  }

  std::vector<T> get(unsigned count) {
    std::vector<T> result;
    result.reserve(count);
    for (int i = 0; i < count; ++i) {
      std::uniform_int_distribution<unsigned> uni(0, root->weight);
      auto roll = uni(gen32);
      result.push_back(leafValues[search(root, roll, 0)]);
    }
    clean(root);
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

int main() {
  std::vector<std::pair<int, unsigned>> inputVec{
    {0, 50},
    {1, 40},
    {3, 120},
    {4, 140},
    {5, 141},
    {8, 10}
  };
  Tree<int> t(inputVec);
  auto vals = t.get(4);
  print(vals);
  // std::cout << t.search) << '\n';
  return 0;
}
