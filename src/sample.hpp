#ifndef SAMPLE_HPP
#include <vector>
#include <algorithm>
#include <random>

template <typename T, typename RNG>
std::vector<T> sample_without_replacement(T k, T n, RNG &&rng) {
  std::vector<T> v(n);
  std::iota(begin(v), end(v), 0);

  std::shuffle(v.begin(), v.end(), rng);
  std::vector<T> w(k);
  std::copy_n(begin(v), k, begin(w));
  return (w);
}
#endif
