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

static void get_matches_pavetic(
    const string& a, const string& b,
    const int k, vector<pair<int, int> >* matches) {
  assert(matches != NULL);
  matches->clear();

  auto mapping = remap_characters(a, b);
  vector<int> aid = mapping.first;
  int alphabet_size = mapping.second;

  // We assume: alphabet_size ** k < 2 ** 64,
  // in the case this does not hold, the entire
  // get_matches function probably has to be
  // reimplemented using non-perfect hashing or
  // suffix arrays.
  if (k * log(alphabet_size) >= 64 * log(2)) {
    fprintf(stderr, "We assume that alphabet_size ** k <\
2 ** 64.\nPlease see lcskpp.cpp for more information.");
    exit(1);
  }

  typedef unordered_multimap<uint64_t, int> MatchIndexType;
  unique_ptr<MatchIndexType> match_index =
    unique_ptr<MatchIndexType>(new MatchIndexType());

  uint64_t hash_mod = 1;
  for (int i = 0; i < k; ++i) hash_mod *= alphabet_size;

  if (alphabet_size == 4) {
    assert(hash_mod == (1LL<<(2*k)));
  }

  uint64_t rolling_hash = 0;
  for (int i = 0; i < a.size(); ++i) {
    rolling_hash = rolling_hash * alphabet_size + aid[a[i]];
    rolling_hash %= hash_mod;

    if (i+1 >= k) {
      match_index->insert(
        MatchIndexType::value_type(rolling_hash, i-k+1));
    }
  }

  rolling_hash = 0;
  for (int i = 0; i < b.size(); ++i) {
    rolling_hash = rolling_hash * alphabet_size + aid[b[i]];
    rolling_hash %= hash_mod;

    if (i+1 >= k) {
      auto positions_in_a = match_index->equal_range(rolling_hash);
      for (auto it = positions_in_a.first; 
           it != positions_in_a.second; ++it) {
        matches->push_back(make_pair(it->second, i-k+1));
      }
    }
  }

  sort(matches->begin(), matches->end());
}

// Assumes v is sorted by _.second and only the
// first 40 bits of _.first are significant.
void radix_sort(vector<pair<uint64_t, int>> &v) {
  const int len = 14;
  const int sz = 1 << len;
  const int mask = sz - 1;
  static int pos[sz + 1];

  vector<pair<uint64_t, int>> w(v.size());

  REP(block, 3) {
    memset(pos, 0, sizeof pos);
    
    for (auto &x : v)
      ++pos[((x.first >> (block * len)) & mask) + 1];

    REP(i, sz)
      pos[i + 1] += pos[i];

    for (auto &x : v)
      w[pos[(x.first >> (block * len)) & mask]++] = x;

    v = w;
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
