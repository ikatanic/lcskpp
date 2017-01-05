#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <functional>
#include <map>
#include <unordered_map>
#include <queue>
#include <memory>
#include <climits>


using namespace std;

// Same as following code in Python:
//   *c = a*b < 2**63
bool product_fits_in_63bits(uint64_t a, uint64_t b, uint64_t* c) {
  if (a <= LLONG_MAX / b) {
    *c = a * b;
    return true;
  }
  return false;
}


// Same as following code in Python:
//   *c = a**b < 2**63
bool power_fits_in_63bits(uint64_t a, uint64_t b, uint64_t* c) {
  *c = 1;
  while (b > 0) {
    if (!product_fits_in_63bits(*c, a, c)) return false;
    b--;
  }
  return true;
}

uint64_t next_pw2(uint64_t x) {
  uint64_t power_2 = 1;
  while (power_2 < x) power_2 *= 2;
  return power_2;
}


// Sorts vector of pairs v, by v.first using radix sort.
// Argument maks should be equal to largest v.first.
void radix_sort(vector<pair<uint64_t, int>> &v, uint64_t maks) {
  const int len = 9;
  const int sz = 1 << len;
  const int mask = sz - 1;
  static short pos[sz + 1];

  vector<pair<uint64_t, int>> w(v.size());

  int n_blocks = 0;
  while (maks > 1) {
    n_blocks++;
    maks >>= len;
  }
  
  for (int block = 0; block < n_blocks; ++block) {
    memset(pos, 0, sizeof pos);
    
    for (auto &x : v)
      ++pos[((x.first >> (block * len)) & mask) + 1];

    for (int i = 0; i < sz; ++i)
      pos[i + 1] += pos[i];

    for (auto &x : v)
      w[pos[(x.first >> (block * len)) & mask]++] = x;

    v.swap(w);
  }
}

// Sorts vector of integers using radix sort.
// Argument maks should be equal to largest integer in the vector.
void radix_sort(vector<uint64_t> &v, uint64_t maks) {
  const int len = 9;
  const int sz = 1 << len;
  const int mask = sz - 1;
  static short pos[sz + 1];

  vector<uint64_t> w(v.size());

  int n_blocks = 0;
  while (maks > 1) {
    n_blocks++;
    maks /= 2;
  }

  int shift = 0;
  while (n_blocks > 0) {
    int len = min(9, n_blocks);
    int sz = 1<<len;
    int mask = sz - 1;
    
    memset(pos, 0, sizeof pos);
    
    for (auto &x : v)
      ++pos[((x >> shift) & mask) + 1];

    for (int i = 0; i < sz; ++i)
      pos[i + 1] += pos[i];

    for (auto &x : v)
      w[pos[(x >> shift) & mask]++] = x;
    shift += len;
    n_blocks -= len;
    
    v.swap(w);
  }
}


// Finds all k-matches in given pair of strings.
void calc_matches(const string& a, const string& b, int k,
                 vector<pair<int, int>>* matches) {
  // First, remap characters to interval [0, sigma>,
  // where sigma is alphabet size.
  vector<int> map(256, -1);
  int sigma = 0;
  int n = a.size();
  int m = b.size();
  for (int i = 0; i < n; ++i) {
    if (map[a[i]] == -1) {
      map[a[i]] = sigma++;
    }
  }
  for (int i = 0; i < m; ++i) {
    if (map[b[i]] == -1) {
      map[b[i]] = sigma++;
    }
  }
  // sigma_to_k is number of possible k length strings
  uint64_t sigma_to_k;
  if (!power_fits_in_63bits(sigma, k, &sigma_to_k)) {
    fprintf(stderr, "This implementation works only when sigma^k < 2^64.");
    return;
  }
  // increase sigma_to_k to the next power of two, for cheaper mod operation
  sigma_to_k = next_pw2(sigma_to_k);
  uint64_t mod_mask = sigma_to_k - 1;

  
  // Now to find actual pairs, we use perfect hash-table if sigma_to_k is small
  // enough, and otherwise:
  //   a) throw k-length substrings hashes from both string, into 16 buckets,
  //      based on first 4 bits of the hash.
  //   b) radix sort each bucket
  //   c) find consecutive hashes of the same value and save pairs of hashes
  //      from different strings.
  
  if (sigma_to_k <= (1<<20) && sigma_to_k <= 4*(n + m)) {
    // for each value of hash build a linked list containing such substrings
    // from the second string
    vector<int> last(sigma_to_k, -1);
    vector<int> prev(m);
    uint64_t current_hash = 0;
    for (int i = 0; i < m; ++i) {
      current_hash = (current_hash * sigma + map[(unsigned char)b[i]]) & mod_mask;
      if (i >= k-1) {
        prev[i] = last[current_hash];
        last[current_hash] = i;
      }
    }

    // now for each substring of the first string, fetch equal substrings from
    // the linked list
    current_hash = 0;
    for (int i = 0; i < n; ++i) {
      current_hash = (current_hash * sigma + map[(unsigned char)a[i]]) & mod_mask;
      if (i >= k-1) {
        int sz = matches->size();
        for (int j = last[current_hash]; j != -1; j = prev[j]) {
          matches->push_back({i, j});
        }
        reverse(matches->begin() + sz, matches->end());
      }
    }
  } else {
    // Now we need to store pairs (hash, position in string), if both things
    // can fit into single 64-bit integer, we do so. Otherwise we use std::pair.

    uint64_t _tmp;
    if (product_fits_in_63bits(sigma_to_k, n + m, &_tmp)) {
      const int P = 16;
      vector<uint64_t> hashes[P];
      for (int p = 0; p < P; ++p) {
        hashes[p].reserve((n + m) / P);
      }
      
      uint64_t current_hash = 0;
      for (int i = 0; i < n; ++i) {
        current_hash = (current_hash * sigma + map[(unsigned char)a[i]]) & mod_mask;
        if (i >= k-1) hashes[current_hash % P].push_back(i*sigma_to_k + current_hash/P);
      }

      current_hash = 0;
      for (int i = 0; i < m; ++i) {
        current_hash = (current_hash * sigma + map[(unsigned char)b[i]]) & mod_mask;
        if (i >= k-1) hashes[current_hash % P].push_back((n + i)*sigma_to_k + current_hash/P);
      }

      for (int p = 0; p < P; ++p) {
        radix_sort(hashes[p], sigma_to_k / P);
  
        int sz = hashes[p].size();
        for (int i = 0, j = 0; i < sz; i = j) {
          int s = 0;
          for (j = i + 1; j < sz && (hashes[p][j] & mod_mask) == (hashes[p][i] & mod_mask); ++j);
          if (j - i > 1) {
            int s = i;
            while (s < j && (hashes[p][s] / sigma_to_k) < n) ++s;

            for (int k1 = i; k1 < s; ++k1) {
              for (int k2 = s; k2 < j; ++k2) {
                matches->push_back({hashes[p][k1] / sigma_to_k, (hashes[p][k2] / sigma_to_k) - n});
              }
            }
          }
        }
      }
    } else {
      const int P = 16;
      vector<pair<uint64_t, int>> hashes[P];
      for (int p = 0; p < P; ++p) {
        hashes[p].reserve((n + m) / P);
      }
    
      uint64_t current_hash = 0;
      for (int i = 0; i < n; ++i) {
        current_hash = (current_hash * sigma + map[(unsigned char)a[i]]) & mod_mask;
        if (i >= k-1) hashes[current_hash % P].push_back({current_hash / P, i});
      }

      current_hash = 0;

      for (int i = 0; i < m; ++i) {
        current_hash = (current_hash * sigma + map[(unsigned char)b[i]]) & mod_mask;
        if (i >= k-1) hashes[current_hash % P].push_back({current_hash / P, (n + i)});
      }

      for (int p = 0; p < P; ++p) {
        radix_sort(hashes[p], sigma_to_k / P);
  
        int sz = hashes[p].size();
        for (int i = 0, j = 0; i < sz; i = j) {
          int s = 0;
          for (j = i + 1; j < sz && hashes[p][j].first == hashes[p][i].first; ++j);
          if (j - i > 1) {
            int s = i;
            while (s < j && hashes[p][s].second < n) ++s;

            for (int k1 = i; k1 < s; ++k1) {
              for (int k2 = s; k2 < j; ++k2) {
                matches->push_back({hashes[p][k1].second, hashes[p][k2].second - n});
              }
            }
          }
        }
      }
    }
    

    if (matches->size() < (n + m) / 16) {
      sort(matches->begin(), matches->end());
    } else {
      vector<int> pos(n+1, 0);
      vector<pair<int, int>> tmp(matches->size());
      for (auto& p: *matches) pos[p.first + 1]++;
      for (int i = 0; i < n; ++i)
        pos[i+1] += pos[i];
      for (auto& p: *matches) tmp[ pos[p.first]++ ] = p;
      matches->swap(tmp);
    }
  }
}
