#include <vector>
#include <iostream>
#include <chrono>
#include <drawtree.hpp>
#include <unordered_set>

void print(auto iterable) {
  for (const auto& x : iterable) {
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

bool tester(uint_fast64_t n) {
  std::vector<uint_fast64_t> inputVec;
  for (uint_fast64_t i = 0; i < n; ++i) {
    inputVec.emplace_back((uint_fast64_t)i);
  }
  DrawTree t(inputVec);
  auto res = t.get(n - 1);
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
  constexpr unsigned total{100000};
  constexpr unsigned draw{50000};
  // std::mt19937 gen32{std::random_device()()};
  std::vector<uint_fast64_t> weights(total);
  std::iota(weights.begin(), weights.end(), 1ull);
  DrawTree t(weights);
  std::cout << measure([&t]() { t.get(draw); }) << '\n';
  std::cout << measure([]() { BobFloydAlgo(draw, total); }) << '\n';
  // std::cout << t.nodes[t.root] << '\n';
  // for (int i = 1; i < 4000; ++i) {
  //   std::cout << i << '\n';
  //   if (!tester(i)) {
  //     exit(1);
  //   }
  // }
  if (!tester(10000ull * 100000ull)) {
    std::cout << " FAIL!!! " << '\n';
  }
  return 0;
}
