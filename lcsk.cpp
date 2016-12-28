// lcsk algoritmi

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

// k-matchevi u O(nm), matches[i] sadrzi j, ako X[i-k+1,i] == Y[j-k+1,j]
void calc_matches_slow(const string &a, const string &b, int k, vector<vector<int>> &matches) {
  int n = a.size();
  int m = b.size();

  matches.resize(n + 1);
  vector<vector<int>> g(n + 1, vector<int>(m + 1, 0));
  REP(i, n) REP(j, m) {
    if (a[i] == b[j]) {
      if (i && j) g[i][j] = g[i-1][j-1] + 1;
      else g[i][j] = 1;
    }

    if (g[i][j] >= k) 
      matches[i].push_back(j);
  }
}

// obican dp: O(nm)
int lcsk_dp(const string& a, const string& b, int k) {
  int n = a.size();
  int m = b.size();

  vector<vector<int>> matches;
  calc_matches_slow(a, b, k, matches);

  vector<vector<int>> f(n + 1, vector<int>(m + 1, 0));
  
  REP(i, n+1) {
    int jp = 0;
    REP(j, m+1) {
      if (i) f[i][j] = max(f[i-1][j], f[i][j]);
      if (j) f[i][j] = max(f[i][j-1], f[i][j]);

      while (jp < (int)matches[i].size() && matches[i][jp] < j) ++jp;
      if (jp < (int)matches[i].size() && matches[i][jp] == j) {
	if (i >= k && j >= k)
	  f[i][j] = max(f[i][j], f[i-k][j-k] + 1);
	else
	  f[i][j] = 1;
      }
    }
  }

  return f[n][m];
}

int lcsk_better(const string& a, const string& b, int k) {
  int n = a.size();
  int m = b.size();

  vector<vector<int>> matches;
  calc_matches_slow(a, b, k, matches); // brute za sad!!!

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
  {"dp", lcsk_dp},
  {"better?", lcsk_better},
};

string gen_random_string(int n, int sigma) {
  string ret;
  REP(i, n) ret.push_back((rand() % sigma) + 'A');
  return ret;
}

int main(void) {
  srand(123456789);

  map<string, double> times;

  int T = 10;
  REP(t, T) {
    int N = 2000;
    int S = 5;
    
    string A = gen_random_string(N, S);
    string B = gen_random_string(N, S);
    int k = rand() % 5 + 1;

    int lcsk_len = lcsk_dp(A, B, k);
    
    for (auto& solver : solvers) {
      times[solver.first] -= clock();
      int solver_lcsk_len = solver.second(A, B, k);
      times[solver.first] += clock();
      if (solver_lcsk_len != lcsk_len) {
        puts("BUG");
        TRACE(lcsk_len _ solver.first _ solver_lcsk_len);
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

