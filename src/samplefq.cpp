#include <unistd.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>
#include "io.hpp"

using namespace std;

void count_seq(istream &is, size_t &cnt) {
  string line;
  while (not is.eof() and getline(is, line))
    if (line[0] == '@') {
      cnt++;
      getline(is, line);
      getline(is, line);
      getline(is, line);
    }
}

template <typename T>
vector<T> sample_without_replacement(T k, T n) {
  vector<T> v(n);
  std::iota(begin(v), end(v), 0);

  std::random_device rd;
  std::mt19937 g(rd());

  std::shuffle(v.begin(), v.end(), g);
  vector<T> w(k);
  copy_n(begin(v), k, begin(w));
  return (w);
}

void print_out(istream &is, const vector<size_t> &idxs, ostream &os) {
  string line;
  size_t idx = 0;
  auto it = begin(idxs);
  while (it != end(idxs) and getline(is, line))
    if (line[0] == '@') {
      if (idx++ == *it) {
        it++;
        os << line << endl;
        getline(is, line);
        os << line << endl;
        getline(is, line);
        os << line << endl;
        getline(is, line);
        os << line << endl;
      }
    }
}

int main(int argc, char **argv) {
  if (argc < 3 or argc > 4) {
    cerr << "Arguments: n PATH1 [PATH2]" << endl;
    return EXIT_FAILURE;
  }

  size_t k = atoi(argv[1]);
  string path1 = argv[2];
  string path2 = "";
  if (argc == 4) path2 = argv[3];

  size_t n = 0;
  parse_file(path1, count_seq, n);

  if (n < k) {
    cerr << "Error: cannot sample " << k << " sequence when there are only "
         << n << " sequences in " << path1 << endl;
    return EXIT_FAILURE;
  }
  // cout << "# there are " << n << " sequences" << endl;

  vector<size_t> idxs = sample_without_replacement(k, n);

  sort(begin(idxs), end(idxs));

#pragma omp sections
  {
    { parse_file(path1, print_out, idxs, cout); }
#pragma omp section
    {
      if (path2 != "") parse_file(path2, print_out, idxs, cerr);
    }
  }

  return EXIT_SUCCESS;
}
