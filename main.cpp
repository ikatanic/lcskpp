// Copyright 2017: I. Katanic, G. Matula

#include <iostream>
#include <fstream>
#include "lcskpp.h"

using namespace std;

int main(int argc, char** argv) {
  if (argc != 5) {
    printf(
      "Compute LCSk++ of two plain texts.\n\n"
      "Usage: ./main k input1 input2 output\n\n"
      "Example: ./main 4 test/tests/test.1.A test/tests/test.1.B out\n"
      "finds LCS4++ of files `test/tests/test.1.A` and `test/tests/test.1.B`\n"
      "and writes it to `out`\n"
    );
    return 0;
  };


  int k = stoi(argv[1]);
  ifstream infile1(argv[2]);
  string A;
  getline(infile1, A);
  
  ifstream infile2(argv[3]);
  string B;
  getline(infile2, B);

  printf("Sequence 1 length: %d\n", (int)A.size());
  printf("Sequence 2 length: %d\n", (int)B.size());
  printf("Computing LCSk++..\n");
  
  vector<pair<int, int>> recon;
  int length = lcskpp(A, B, k, &recon);

  printf("LCSk++ length: %d\n", length);
  
  auto r = freopen(argv[4], "w", stdout);
  for (auto& p: recon) {
    putchar(A[p.first]);
  }
  return 0;
}
