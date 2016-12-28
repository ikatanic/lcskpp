// lcskpp algoritmi

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <functional>
#include <map>
#include <queue>

using namespace std;

#define FOR(i, a, b) for (int i = (a); i < (b); ++i)
#define REP(i, n) FOR(i, 0, n)
#define TRACE(x) cout << #x << " = " << x << endl
#define _ << " _ " <<

typedef long long llint;
const int inf = 1e9;

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
  int tot = 0;
  for (int i = 0; i < (int)a.size(); ++i) {
    current_hash = (current_hash * sigma + map[(unsigned char)a[i]]) % sigma_to_k;
    if (i >= k-1) {
      for (auto it = lower_bound(b_hashes.begin(), b_hashes.end(), make_pair(current_hash, -1));
           it != b_hashes.end() && it->first == current_hash;
           ++it) {
        matches[i].push_back(it->second);
        tot++;
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


// obican dp: O(nm)
int lcskpp_dp(const string& a, const string& b, int k) {
  int n = a.size();
  int m = b.size();

  vector<vector<int>> matches = calc_matches(a, b, k);
  vector<vector<int>> f(n, vector<int>(m, 0));

  REP(i, n) {
    int jp = 0;
    int cont_ptr = 0;
    REP(j, m) {
      if (i) f[i][j] = max(f[i-1][j], f[i][j]);
      if (j) f[i][j] = max(f[i][j-1], f[i][j]);

      while (jp < (int)matches[i].size() && matches[i][jp] < j) ++jp;
      if (jp < (int)matches[i].size() && matches[i][jp] == j) {
	if (i >= k && j >= k)
	  f[i][j] = max(f[i][j], f[i-k][j-k] + 1);
	else
	  f[i][j] = 1;

        if (i > 0) {
          while (cont_ptr < (int)matches[i-1].size() && matches[i-1][cont_ptr] < j-1) cont_ptr++;
          if (cont_ptr < (int)matches[i-1].size() && matches[i-1][cont_ptr] == j-1) {
            f[i][j] = max(f[i][j], f[i-1][j-1] + 1);
          }
        }
      }
    }
  }

  return f[n-1][m-1];
}


// racuna LCSK zasad 
int lcskpp_better(const string& a, const string& b, int k) {
  int n = a.size();
  //  int m = b.size();

  vector<vector<int>> matches = calc_matches(a, b, k);

  // trenutno nisam u stanju maknut deque...
  // znaci zelim imat pristup staroj verziji MinYPrefix, pa pamtim
  // stare vrijednosti za svaki element (ograniceni broj doduse).

  struct Cell {
    deque<pair<int,int>> dq;

    Cell() { dq.push_back({-inf, +inf}); }

    int get_old(int d) {
      while (dq.size() > 1 && dq[1].first <= d) dq.pop_front();
      return dq[0].second;
    }

    int get_new() {
      return dq.back().second;
    }

    void set(int i, int v) {
      dq.push_back({i, v});
    }
  };

  vector<Cell> MinYPrefix(n + 1);
  MinYPrefix[0].dq[0].second = -1e9;

  int r = 0;

  REP(i, n) {
    for (auto jt = matches[i].rbegin();
	 jt != matches[i].rend();
	 ++jt) {

      // nadji match na koji se mogu nastavit, dakle
      // gledam MinYPrefix iz iteracije i - k, pozicije
      // manje ili jednake *jt - k.

      int lo = 0, hi = n;
      while (lo < hi) {
	int mid = (lo + hi) / 2;
	if (MinYPrefix[mid].get_old(i - k) < *jt - k + 1)
	  lo = mid + 1;
	else
	  hi = mid;
      }

      int l = lo - 1;

      // probam popravit trenutni MinYPrefix.
      
      if (*jt < MinYPrefix[l + 1].get_new())
	MinYPrefix[l + 1].set(i, *jt);

      if (l == r) ++r;
    }
  }

  return r;
}

typedef function<int (const string&, const string&, int)> solver_t;

map<string, solver_t> solvers = {
  {"dp", lcskpp_dp},
  {"better?", lcskpp_better},
};

string gen_random_string(int n, int sigma) {
  string ret;
  REP(i, n) ret.push_back((rand() % sigma) + 'A');
  return ret;
}

int main(void) {
  srand(123456789);

  map<string, double> times;

  int T = 20;
  REP(t, T) {
    int N = 3000;
    int S = 4;
    
    string A = gen_random_string(N, S);
    string B = gen_random_string(N, S);
    int k = rand() % 5 + 2;

    int lcskpp_len = lcskpp_dp(A, B, k);
    
    for (auto& solver : solvers) {
      times[solver.first] -= clock();
      int solver_lcskpp_len = solver.second(A, B, k);
      times[solver.first] += clock();
      if (solver_lcskpp_len != lcskpp_len) {
        puts("BUG");
        TRACE(lcskpp_len _ solver.first _ solver_lcskpp_len);
        return 0;
      }
    }
    printf("done %d/%d\n", t+1, T);
  }
  printf("\n\n");
  
  for (auto& time: times) {
    time.second /= CLOCKS_PER_SEC;
    time.second /= T;
    printf("%s -> %.2lf\n", time.first.c_str(), time.second);
  }
  
  return 0;
}

