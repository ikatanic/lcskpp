#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <fstream>
#include <functional>
#include <map>
#include <unordered_map>
#include <queue>
#include <memory>

#include "../lcskpp.h"
#include "../../lcskpp_pavetic/lcskpp/lcskpp.h"

#define FOR(i, a, b) for (int i = (a); i < (b); ++i)
#define REP(i, n) FOR(i, 0, n)
#define TRACE(x) cout << #x << " = " << x << endl
#define _ << " _ " <<

using namespace std;

int main(int argc, char** argv) {
  if (argc != 5) {
    puts("Usage: ./pavetic k input1 input2 output");
    puts("Ex.: ./pavetic 4 tests/test.1.A tests/test.1.B out");
    return 0;
  };


  int k = stoi(argv[1]);
  ifstream infile1(argv[2]);
  string A;
  getline(infile1, A);
  
  ifstream infile2(argv[3]);
  string B;
  getline(infile2, B);

  vector<pair<int, int>> recon;
  int length;
  lcskpp(A, B, k, &length, &recon);

  auto r = freopen(argv[4], "w", stdout);
  printf("%d\n", length);
  for (auto& p: recon) {
    putchar(A[p.first]);
  }
  return 0;
}
