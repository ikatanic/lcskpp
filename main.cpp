#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>

using namespace std;

// Maps characters found in strings a and b to interval [0, S>, where S is
// number of unique characters.
// Returns mapping and S.
pair<vector<int>, int> remap_characters(const string& a, const string& b) {
  vector<int> char_id(256, -1);
  int char_idx = 0;
  for (const unsigned char& c: a) {
    if (char_id[c] == -1) {
      char_id[c] = char_idx++;
    }
  }
  for (const unsigned char& c: b) {
    if (char_id[c] == -1) {
      char_id[c] = char_idx++;
    }
  }
  return {char_id, char_idx};
}

// Takes strings a and b as inputs.
// Returns sorted list of all k-matches.
// K-match is a pair (i, j) such that a[i..i+k> == b[j..j+k>.
// Time: O(n log n + m log m + r) where n = |a|, m = |b|, r = |k-matches|.
vector<pair<int, int>> find_kMatches(const string& a, const string& b, int k) {
  // First, remap characters to interval [0, c>
  auto mapping = remap_characters(a, b);
  vector<int> map = mapping.first;
  int sigma = mapping.second;

  uint64_t sigma_to_k = 1;
  for (int i = 0; i < k; ++i) {
    sigma_to_k *= sigma;
  }

  vector<pair<uint64_t, int>> b_hashes;
  uint64_t current_hash = 0;
  for (int i = 0; i < (int)b.size(); ++i) {
    current_hash = current_hash * sigma + map[(unsigned char)b[i]];
    if (i >= k-1) {
      b_hashes.push_back({current_hash, i});
      current_hash -= sigma_to_k * map[(unsigned char)b[i]];
    }
  }

  sort(b_hashes.begin(), b_hashes.end());

  vector<pair<int, int>> kMatches;
  current_hash = 0;
  for (int i = 0; i < (int)a.size(); ++i) {
    current_hash = current_hash * sigma + map[(unsigned char)a[i]];
    if (i >= k-1) {
      for (auto it = lower_bound(b_hashes.begin(), b_hashes.end(), make_pair(current_hash, -1));
           it != b_hashes.end() && it->first == current_hash; ++it) {
        kMatches.push_back({i, it->second});
      }
      current_hash -= sigma_to_k * map[(unsigned char)a[i]];
    }
  }

  return kMatches;
}

int main(void) {
  
  return 0;
}
