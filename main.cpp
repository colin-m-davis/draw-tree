#include <vector>
#include <iostream>
#include <utility>
#include <random>
#include <iterator>
#include <algorithm>

// template <typename T>
// concept Ordered = requires(T&& a, T&& b) {
//   { a == b } -> std::convertible_to<bool>;
//   { a < b } -> std::convertible_to<bool>;
// };

template <typename T>
class Tree {
public:
  typedef std::vector<std::pair<T, unsigned>> ItemData;
  struct Node {
    unsigned weight = 0;
    bool activated = false;
  };

  unsigned numItems;
  unsigned depth;
  std::vector<Node> nodes;
  ItemData items;
  typedef typename std::vector<Node>::iterator TreeIt;
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

  unsigned getIndex(TreeIt node) {
    return std::distance(root, node);
  }

  TreeIt getLeftChild(TreeIt parent) {
    return std::next(root, getIndex(parent) * 2 + 1);
  }

  TreeIt getRightChild(TreeIt parent) {
    return std::next(getLeftChild(parent));
  }

  TreeIt getParent(TreeIt child) {
    return std::next(root, (getIndex(child) - 1) / 2);
  }

  unsigned fillWeights(TreeIt node) {
    if (getIndex(node) < nodes.size() / 2) {
      node->weight = fillWeights(getLeftChild(node)) + fillWeights(getRightChild(node));
    }
    return node->weight;
  }

  // returns getIndex of result in leafValues
  unsigned search(TreeIt node, unsigned roll, int level) {
    std::cout << "search " << getIndex(node) << '\n';
    if (level == depth) {
      node->activated = false;
      return getIndex(node) - nodes.size() / 2;
    } else {
      auto left = getLeftChild(node);
      auto right = getRightChild(node);
      // std::cout << "generated " << roll << " out of " << left->weight << " / " << node->weight << '\n';
      const auto result = roll <= left->weight ?
        search(left, roll, level + 1) :
        search(right, roll - left->weight, level + 1);
      if (!(left->activated || right->activated)) {
        node->activated = false;
      }
      return result;
    }
  }

  void bubble(TreeIt node, int offset) {
    node->weight += offset;
    if (getIndex(node) > 0) {
      bubble(getParent(node), offset);
    }
  }

// public:
  Tree(const ItemData& inputVec) :
    numItems(inputVec.size()),
    items(inputVec),
    depth(getDepth(inputVec.size())),
    nodes(getNumNodes(inputVec.size()), Node()),
    root(nodes.begin())
  {
    auto lastRowIt = std::next(root, nodes.size() / 2);
    for (auto inputIt = inputVec.cbegin(); inputIt < inputVec.cend(); ++inputIt) {
      *(lastRowIt++) = Node{inputIt->second, true};
    }
    for (; lastRowIt < nodes.end(); ++lastRowIt) {
      *lastRowIt = Node{};
    }
    fillWeights(root);
  }

  std::mt19937 gen32{std::random_device()()};

  void toggle(unsigned i, bool on = true) {
    const auto leaf = nodes.begin() + (nodes.size() / 2) + i;
    const auto offset = (on ? 1 : -1) * static_cast<int>(items[i].second);
    bubble(leaf, offset);
  }

  std::vector<T> get(unsigned count) {
    std::vector<unsigned> indices;
    indices.reserve(count);
    for (int iterations = 0; iterations < count; ++iterations) {
      std::uniform_int_distribution<unsigned> dist(0, root->weight);
      auto roll = dist(gen32);
      auto i = search(root, roll, 0);
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

int main() {
  std::vector<std::pair<int, unsigned>> inputVec{
    {0, 1},
    {0, 1},
        {0, 1},
            {0, 1},
                {0, 1},
  };
  Tree<int> t(inputVec);
  auto vals = t.get(4);
  print(vals);
  // std::cout << t.search << '\n';
  return 0;
}
