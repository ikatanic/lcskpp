#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <functional>
#include <map>
#include <unordered_map>
#include <queue>
#include <memory>


#define FOR(i, a, b) for (int i = (a); i < (b); ++i)
#define REP(i, n) FOR(i, 0, n)
#define TRACE(x) cout << #x << " = " << x << endl
#define _ << " _ " <<

using namespace std;

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

// Assumes v is sorted by _.second and only the
// first 40 bits of _.first are significant.
void radix_sort(vector<pair<uint64_t, int>> &v) {
  const int len = 10;
  const int sz = 1 << len;
  const int mask = sz - 1;
  static int pos[sz + 1];

  vector<pair<uint64_t, int>> w(v.size());
  
  REP(block, 4) {
    memset(pos, 0, sizeof pos);
    
    for (auto &x : v)
      ++pos[((x.first >> (block * len)) & mask) + 1];

    REP(i, sz)
      pos[i + 1] += pos[i];

    for (auto &x : v)
      w[pos[(x.first >> (block * len)) & mask]++] = x;

    v.swap(w);
  }
}

// Assumes v is sorted by _.second and only the
// first 40 bits of _.first are significant.
void radix_sort(vector<uint64_t> &v, uint64_t maks) {
  const int len = 9;
  const int sz = 1 << len;
  const int mask = sz - 1;
  static short pos[sz + 1];

  vector<uint64_t> w(v.size());

  int BLOCKS = 0;
  while (maks > 1) {
    BLOCKS++;
    maks >>= len;
  }
  
  REP(block, BLOCKS) {
    memset(pos, 0, sizeof pos);
    
    for (auto &x : v)
      ++pos[((x >> (block * len)) & mask) + 1];

    REP(i, sz)
      pos[i + 1] += pos[i];

    for (auto &x : v)
      w[pos[(x >> (block * len)) & mask]++] = x;

    v.swap(w);
  }
}


void calc_matches(const string& a, const string& b, int k, vector<pair<int, int>>* matches) {
  //  First, remap characters to interval [0, c>
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
  
  uint64_t sigma_to_k = 1;
  for (int i = 0; i < k; ++i) {
    sigma_to_k *= sigma;
  }

  if (sigma_to_k <= (1<<16)) {
    vector<int> last(sigma_to_k, -1);
    vector<int> prev(m);
    uint64_t current_hash = 0;
    for (int i = 0; i < m; ++i) {
      current_hash = (current_hash * sigma + map[(unsigned char)b[i]]) % sigma_to_k;
      if (i >= k-1) {
        prev[i] = last[current_hash];
        last[current_hash] = i;
      }
    }


    current_hash = 0;
    for (int i = 0; i < n; ++i) {
      current_hash = (current_hash * sigma + map[(unsigned char)a[i]]) % sigma_to_k;
      if (i >= k-1) {
        int sz = matches->size();
        for (int j = last[current_hash]; j != -1; j = prev[j]) {
          matches->push_back({i, j});
        }
        reverse(matches->begin() + sz, matches->end());
       }
    }
  } else {
    const int P = 16;
    vector<uint64_t> hashes[P];
    REP(i, P) hashes[i].reserve((n + m) / P);
    
    uint64_t current_hash = 0;
    for (int i = 0; i < n; ++i) {
      current_hash = (current_hash * sigma + map[(unsigned char)a[i]]) & (sigma_to_k - 1);
      if (i >= k-1) hashes[current_hash % P].push_back(current_hash / P + (i*1LL << 40));
    }

    current_hash = 0;

    for (int i = 0; i < m; ++i) {
      current_hash = (current_hash * sigma + map[(unsigned char)b[i]]) & (sigma_to_k - 1);
      if (i >= k-1) hashes[current_hash % P].push_back(current_hash / P + ((i + n)*1LL << 40));
    }

    uint64_t mask = (1LLU<<40) - 1;
    for (int p = 0; p < P; ++p) {
      radix_sort(hashes[p], sigma_to_k / P);
  
      int sz = hashes[p].size();
      for (int i = 0, j = 0; i < sz; i = j) {
        int s = 0;
        for (j = i + 1; j < sz && (hashes[p][j]&mask) == (hashes[p][i]&mask); ++j);
        if (j - i > 1) {
          int s = i;
          while (s < j && (hashes[p][s] >> 40) < n) ++s;

          FOR(k1, i, s) FOR(k2, s, j) {
            matches->push_back({(hashes[p][k1]) >> 40, (hashes[p][k2] >> 40) - n});
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
      REP(i, n) pos[i+1] += pos[i];
      for (auto& p: *matches) tmp[ pos[p.first]++ ] = p;
      matches->swap(tmp);
    }
  }
}
