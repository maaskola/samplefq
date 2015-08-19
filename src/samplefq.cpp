#include <unistd.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>

size_t count_seq(istream &ifs) {
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
  vector v(N);
  std::iota(begin(N), end(N), 0);

  std::random_device rd;
  std::mt19937 g(rd());

  std::shuffle(v.begin(), v.end(), g);
  vector<T> w(n);
  copy_n(begin(v), n, begin(w));
  return(w);
}

void print_out(const string &path, const vector<size_t> idxs, ostream &os) {
  string line;
  ifstream ifs(path);
  size_t idx = 0;
  auto it = begin(idxs);
  while(getline(ifs, line))
    if(line[0] == '@') {
      if(idx++ == *it) {
        it++;
        os << line << endl;
        getline(ifs, line);
        os << line << endl;
        getline(ifs, line);
        os << line << endl;
        getline(ifs, line);
        os << line << endl;
      }
    }
}

int main(int argc, char **argv) {

  int n = atoi(argv[1]);

  vector<string> paths;
  for(size_t i = 2; i < argc; ++i)
    paths.push_back(argv[i]);

  const size_t N = count_seq(ifstream(paths[0]));

  vector<size_t> idxs = sample_without_replacment(n, N);

  sort(begin(idxs), end(idxs));

  print_out(path[0], idxs, cout);
  print_out(path[1], idxs, cerr);

  return EXIT_SUCCESS;
}
