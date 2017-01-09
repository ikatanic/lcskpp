#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>

#include "../lcskpp.h"
#include "../../lcskpp_pavetic/lcskpp/lcskpp.h"

#define FOR(i, a, b) for (int i = (a); i < (b); ++i)
#define REP(i, n) FOR(i, 0, n)
#define TRACE(x) cout << #x << " = " << x << endl
#define _ << " _ " <<

using namespace std;

const int N = 10000;
const int S = 4;

bool indicator(double p) {
  int r = rand() % 2;
  if (p >= 0.5)
    return r ? indicator(2 * p - 1) : true;
  else
    return r ? false : indicator(2 * p);
}

string gen_random_string(int n, int sigma) {
  string ret;
  REP(i, n) ret.push_back((rand() % sigma) + 'A');
  return ret;
}

string gen_with_similarity(const string &base, int sigma, double p) {
  string ret = base;
  REP(i, (int)ret.size())
    if (!indicator(p)) {
      do {
	ret[i] = (rand() % sigma) + 'A';
      } while (base[i] == ret[i]);
    }
  return ret;
}

int main(int argc, char **argv)
{
  if (argc != 5) {
    puts("Usage: ./gen N sigma p_similar(%) filename");
    puts("Ex.: ./gen 10000 6 60");
    exit(0);
  }

  srand(123445678);
  
  int N = atoi(argv[1]);
  int sigma = atoi(argv[2]);
  int p_similar = atoi(argv[3]);
  string filename = argv[4];

  string A = gen_random_string(N, sigma);
  string B = gen_with_similarity(A, sigma, p_similar / 100.0);

  freopen(filename.c_str(), "w", stdout);
  printf(">Sequence1\n");
  for (int i = 0; i < (int)A.size(); i += 80) {
    string line = A.substr(i, 80);
    printf("%s\n", line.c_str());
  }
  printf(">Sequence2\n");
  for (int i = 0; i < (int)B.size(); i += 80) {
    string line = B.substr(i, 80);
    printf("%s\n", line.c_str());
  }

  return 0;
}
