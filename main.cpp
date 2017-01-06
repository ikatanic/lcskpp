#include <iostream>
#include <fstream>
#include "lcskpp.h"

using namespace std;

int main(int argc, char** argv) {
  if (argc != 5) {
    puts("Usage: ./main k input1 input2 output");
    puts("Ex.: ./main 4 test/tests/test.1.A test/tests/test.1.B out");
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
  int length = lcskpp(A, B, k, &recon);


  auto r = freopen(argv[4], "w", stdout);
  printf("%d\n", length);
  for (auto& p: recon) {
    putchar(A[p.first]);
  }
  return 0;
}
