#include <unistd.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>
#include "io.hpp"

using namespace std;

size_t count_seq(istream &&ifs) {
  size_t cnt = 0;
  string line;
  while(getline(ifs, line))
    if(line[0] == '@') {
      cnt++;
      getline(ifs, line);
      getline(ifs, line);
      getline(ifs, line);
    }
  return cnt;
}

template <typename T>
vector<T> sample_without_replacement(T n, T N) {
  vector<T> v(N);
  std::iota(begin(v), end(v), 0);

  std::random_device rd;
  std::mt19937 g(rd());

  std::shuffle(v.begin(), v.end(), g);
  vector<T> w(n);
  copy_n(begin(v), n, begin(w));
  return(w);
}

void print_out(istream &is, const vector<size_t> &idxs, ostream &os) {
  string line;
  size_t idx = 0;
  auto it = begin(idxs);
  while(getline(is, line))
    if(line[0] == '@') {
      if(idx++ == *it) {
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

  if(argc < 3 or argc > 4) {
    cerr << "Arguments: N PATH1 [PATH2]" << endl;
    return EXIT_FAILURE;
  }

  size_t n = atoi(argv[1]);
  string path1 = argv[2];
  string path2 = "";
  if(argc == 4)
    path2 = argv[3];

  const size_t N = count_seq(ifstream(path1));

  vector<size_t> idxs = sample_without_replacement(n, N);

  sort(begin(idxs), end(idxs));

  parse_file(path1, print_out, idxs, cout);
  if(path2 != "")
    parse_file(path2, print_out, idxs, cerr);

  return EXIT_SUCCESS;
}
