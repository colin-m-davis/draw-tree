#include <vector>
#include <iostream>
#include <chrono>
#include <drawtree.hpp>
#include <unordered_set>

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

template <typename T>
bool validate(std::vector<T> vec) {
  std::unordered_set<T> items(vec.begin(), vec.end());
  return vec.size() == items.size();
}

bool test(int n) {
  std::cout << "testing " << n << '\n';
  std::vector<unsigned> inputVec;
  for (int i = 0; i < n; ++i) {
    inputVec.emplace_back(i);
  }
  DrawTree t(inputVec);
  auto res = t.get(n - 1);
  print(res);
  return validate(res);
}

std::unordered_set<int> BobFloydAlgo(int sampleSize, int rangeUpperBound) {
  std::unordered_set<int> sample;
  std::default_random_engine generator;

  for(int d = rangeUpperBound - sampleSize; d < rangeUpperBound; d++)
  {
        int t = std::uniform_int_distribution<>(0, d)(generator);
        if (sample.find(t) == sample.end() )
            sample.insert(t);
        else
            sample.insert(d);
  }
  return sample;
}


int main() {
  // for (int i = 1; i < 10; ++i) {
  //   if (!test(i)) {
  //     std::cout << i << '\n';
  //     return 1;
  //   }
  // }
  constexpr unsigned total = 100000000;
  constexpr unsigned draw = 500000;
  std::mt19937 gen32{std::random_device()()};
  std::vector<unsigned> weights(total);
  for (auto& w : weights) {
    std::uniform_int_distribution<unsigned> dist(1, 102);
    w = dist(gen32);
  }
  DrawTree t(weights);
  std::cout << measure([&t]() { t.get(draw); }) << '\n';
  std::cout << measure([]() { BobFloydAlgo(draw, total); }) << '\n';
  return 0;
}
