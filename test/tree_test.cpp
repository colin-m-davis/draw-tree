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

int main() {
  constexpr unsigned total{1000000};
  constexpr unsigned draw{50000};
  // std::mt19937 gen32{std::random_device()()};
  std::vector<uint_fast64_t> weights(total);
  std::iota(weights.begin(), weights.end(), 1ull);
  DrawTree t(weights);
  std::cout << measure([&t]() {
    for (int i = 0; i < 10; ++i) {
      t.get(draw);
    }
  }) << '\n';
  for (int i = 1; i < 4000; ++i) {
    if (!tester(i)) {
    std::cout << i << '\n';
      exit(1);
    }
  }
  return 0;
}
