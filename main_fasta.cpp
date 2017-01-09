// Copyright 2017: I. Katanic, G. Matula

#include <iostream>
#include <fstream>
#include "lcskpp.h"

using namespace std;

int main(int argc, char** argv) {
  if (argc != 4) {
    printf(
      "Compute LCSk++ of two sequences given in FASTA format.\n\n"
      "Usage: ./main k input output\n\n"
      "Example: ./main 4 input.fa out\n"
      "finds LCS4++ of sequences given in `input.fa` and\n"
      "and writes it to `out`\n"
    );
    return 0;
  };

  int k = stoi(argv[1]);
  ifstream infile(argv[2]);

  vector<string> sequences;
  string line;
  string current_seq;
  bool first = true;
  while (getline(infile, line)) {
    if (!line.size()) continue;
    
    if (line[0] == '>') {
      if (!first) {
        sequences.push_back(current_seq);
      }
      current_seq = "";
      first = false;
    } else if (line[0] == ',') {
      continue;
    } else {
      current_seq += line;
    }
  }
  sequences.push_back(current_seq);

  printf("Read %d sequences.\n", (int)sequences.size());
  if (sequences.size() < 2) {
    printf("Not enough sequences. Aborting.\n");
    return 0;
  }

  string& A = sequences[0];
  string& B = sequences[1];
  
  printf("Computing LCSk++ of first two sequences..\n");
  printf("Sequence 1 length: %d\n", (int)A.size());
  printf("Sequence 2 length: %d\n", (int)B.size());
  
  vector<pair<int, int>> recon;
  int length = lcskpp(sequences[0], sequences[1], k, &recon);
  printf("LCSk++ length: %d\n", length);
  
  auto r = freopen(argv[3], "w", stdout);
  for (auto& p: recon) {
    putchar(A[p.first]);
  }
  return 0;
}
