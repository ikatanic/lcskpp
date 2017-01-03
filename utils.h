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

template<class T>
class FenwickMax {
 public:
  FenwickMax(size_t n) {
    elements_ = std::vector<T> (n+1);
  }
  
  void update(size_t pos, const T& val) {
    ++pos;
    for ( ; pos < elements_.size(); pos += lobit(pos)) {
      elements_[pos] = std::max(elements_[pos], val);
    }
  }

  T get(size_t pos) {
    ++pos;
    T ret = T();
    for ( ; pos > 0; pos -= lobit(pos)) {
      ret = std::max(ret, elements_[pos]);
    }
    return ret;
  }

 private:
  size_t lobit(const size_t& a) { return a&-a; }
  
 private:
  std::vector<T> elements_;
};

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

// Takes strings a and b as inputs. And small number k.
// Returns lists of all k-matches.
// Time: O(n log n + m log m + r) where n = |a|, m = |b|, r = |k-matches|.
vector<vector<int>> calc_matches(const string& a, const string& b, int k) {
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
    current_hash = (current_hash * sigma + map[(unsigned char)b[i]]) % sigma_to_k;
    if (i >= k-1) b_hashes.push_back({current_hash, i});
  }

  radix_sort(b_hashes);
  
  vector<vector<int>> matches(a.size());
  current_hash = 0;
  for (int i = 0; i < (int)a.size(); ++i) {
    current_hash = (current_hash * sigma + map[(unsigned char)a[i]]) % sigma_to_k;
    if (i >= k-1) {
      for (auto it = lower_bound(b_hashes.begin(), b_hashes.end(), make_pair(current_hash, -1));
           it != b_hashes.end() && it->first == current_hash;
           ++it) {
        matches[i].push_back(it->second);
      }
    }
  }
  return matches;
}

vector<vector<int>> calc_matches2(const string& a, const string& b, int k) {
  const int shift = 1 << 30;

  // First, remap characters to interval [0, c>
  auto mapping = remap_characters(a, b);
  vector<int> map = mapping.first;
  int sigma = mapping.second;

  uint64_t sigma_to_k = 1;
  for (int i = 0; i < k; ++i) {
    sigma_to_k *= sigma;
  }

  vector<pair<uint64_t, int>> hashes;
  uint64_t current_hash = 0;
  for (int i = 0; i < (int)a.size(); ++i) {
    current_hash = (current_hash * sigma + map[(unsigned char)a[i]]) % sigma_to_k;
    if (i >= k-1) hashes.push_back({current_hash, i});
  }

  current_hash = 0;

  for (int i = 0; i < (int)b.size(); ++i) {
    current_hash = (current_hash * sigma + map[(unsigned char)b[i]]) % sigma_to_k;
    if (i >= k-1) hashes.push_back({current_hash, i + shift});
  }

  radix_sort(hashes);
  
  vector<vector<int>> matches(a.size());

  int sz = hashes.size();
  for (int i = 0, j = 0; i < sz; i = j) {
    for (j = i + 1; j < sz && hashes[j].first == hashes[i].first; ++j);
    if (j - i <= 1) continue;

    int s = i;
    while (s < j && hashes[s].second < shift) ++s;

    FOR(k1, i, s) FOR(k2, s, j)
      matches[hashes[k1].second].push_back(hashes[k2].second - shift);
  }
  
  return matches;
}



// k-matchevi u O(nm), matches[i] sadrzi j, ako X[i-k+1,i] == Y[j-k+1,j]
vector<vector<int>> calc_matches_slow(const string &a, const string &b, int k) {
  int n = a.size();
  int m = b.size();

  vector<vector<int>> matches(n);
  vector<vector<int>> g(n + 1, vector<int>(m + 1, 0));
  REP(i, n) REP(j, m) {
    if (a[i] == b[j]) {
      if (i && j) g[i][j] = g[i-1][j-1] + 1;
      else g[i][j] = 1;
    }

    if (g[i][j] >= k) 
      matches[i].push_back(j);
  }
  return matches;
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

  const int P = 16;
  vector<uint64_t> hashes[P];
  uint64_t current_hash = 0;
  for (int i = 0; i < n; ++i) {
    current_hash = (current_hash * sigma + map[(unsigned char)a[i]]) % sigma_to_k;
    if (i >= k-1) hashes[current_hash % P].push_back(current_hash / P + (i*1LL << 40));
  }

  current_hash = 0;

  for (int i = 0; i < m; ++i) {
    current_hash = (current_hash * sigma + map[(unsigned char)b[i]]) % sigma_to_k;
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
}


void calc_matches_buckets(const string& a, const string& b, int k, vector<vector<int>>* matches) {
  vector<pair<int, int>> matches_pairs;
  calc_matches(a, b, k, &matches_pairs);
  for (auto& p: matches_pairs) {
    matches->at(p.first).push_back(p.second);
  }
}

