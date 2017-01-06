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
    if (!indicator(p))
      ret[i] = (rand() % sigma) + 'A';
  return ret;
}

int main(int argc, char **argv)
{
  if (argc != 5) {
    puts("Usage: ./gen N sigma p_similar(%) filename");
    puts("Ex.: ./gen 10000 6 60");
    exit(0);
  }

  int N = atoi(argv[1]);
  int sigma = atoi(argv[2]);
  int p_similar = atoi(argv[3]);
  string filename = argv[4];

  string A = gen_random_string(N, sigma);
  string B = gen_with_similarity(A, sigma, p_similar / 100.0);

  string filenameA = filename + ".A";
  string filenameB = filename + ".B";
  freopen(filenameA.c_str(), "w", stdout);
  printf("%s", A.c_str());

  freopen(filenameB.c_str(), "w", stdout);
  printf("%s", B.c_str());

  return 0;
}
