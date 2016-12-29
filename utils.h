#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <functional>
#include <map>
#include <queue>


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

  sort(b_hashes.begin(), b_hashes.end());
  
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