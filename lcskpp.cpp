// lcskpp algoritmi

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <functional>
#include <map>
#include <queue>
#include "utils.h"

using namespace std;

#define FOR(i, a, b) for (int i = (a); i < (b); ++i)
#define REP(i, n) FOR(i, 0, n)
#define TRACE(x) cout << #x << " = " << x << endl
#define _ << " _ " <<

typedef long long llint;

const int inf = 1e9;

// obican dp: O(nmk)
int lcskpp_dp(const string& a, const string& b, int k, const vector<vector<int>>& matches) {
  int n = a.size();
  int m = b.size();

  vector<vector<int>> f(n, vector<int>(m, 0));
  
  REP(i, n) {
    REP(j, m) {
      if (i) f[i][j] = max(f[i-1][j], f[i][j]);
      if (j) f[i][j] = max(f[i][j-1], f[i][j]);

      int q = 0;
      while (i-q >= 0 && j-q >= 0 && a[i-q] == b[j-q] && q < 2*k) {
        q++;
        if (q >= k) {
          f[i][j] = max(f[i][j], (i >= q && j >= q ? f[i-q][j-q] : 0) + q);
        }
      }
    }
  }
  
  return f[n-1][m-1];
}


int lcskpp_better(const string& a, const string& b, int k, const vector<vector<int>>& matches) {
  int n = a.size();
  //  int m = b.size();

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

  map<pair<int, int>, int> point_dp;
  
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

      for (int s = 1; s <= k; ++s) {
        if (*jt < MinYPrefix[l + s].get_new())
          MinYPrefix[l + s].set(i, *jt);
      }
      int my_dp = l+k;
      r = max(r, l + k);

      if (i > 0) {
        if (point_dp.count(make_pair(i-1, *jt-1))) {
          int new_dp = point_dp[make_pair(i-1, *jt-1)] + 1;
          if (new_dp > my_dp) {
            my_dp = new_dp;
            if (*jt < MinYPrefix[new_dp].get_new())
              MinYPrefix[new_dp].set(i, *jt);
          }
          if (my_dp == r+1) r++;
        }
      }

      point_dp[{i, *jt}] = my_dp;
    }

    REP(j, n) {
      // provjeri monotonost
      assert(MinYPrefix[j].get_new() <= MinYPrefix[j+1].get_new());
    }
  }

  return r;
}


// iz papera, modificiran malo
int lcskpp_pavetic(const string& A, const string& B, int k, const vector<vector<int>>& matches_buckets) {
  vector<pair<int, int>> matches;
  for (int i = 0; i < (int)matches_buckets.size(); ++i) {
    for (int j : matches_buckets[i]) {
      matches.push_back({i, j});
    }
  }
  
  vector<tuple<int, int, int> > events;
  events.reserve(2*matches.size());

  int n = 0;
  for (auto it = matches.begin(); it != matches.end(); ++it) {
    int idx = it - matches.begin();
    events.push_back(make_tuple(it->first, it->second, 
				idx+matches.size())); // begin
    events.push_back(make_tuple(it->first+k, it->second+k, idx)); // end
    
    n = max(n, it->first+k);
    n = max(n, it->second+k);
  }
  sort(events.begin(), events.end());

  // Indexed by column, first:dp value, second:index in matches.
  FenwickMax<pair<int, int> > dp_col_max(n);
  vector<int> dp(matches.size());
  vector<int> recon(matches.size());
  vector<int> continues(matches.size(), -1);
  if (k > 1) {
    for (auto curr = matches.begin(); 
         curr != matches.end(); ++curr) {
      auto G = make_pair(curr->first-1, curr->second-1);
      auto prev = lower_bound(matches.begin(), matches.end(), G);
      if (*prev == G) {
	continues[curr-matches.begin()] = prev-matches.begin();
      }
    }
  }

  int best_idx = 0;
  int lcskpp_length = 0;
    
  for (auto event = events.begin(); 
       event != events.end(); ++event) {
    int idx = get<2>(*event) % matches.size();
    bool is_beginning = (get<2>(*event) >= matches.size());
    int i = get<0>(*event);
    int j = get<1>(*event);
    int primary_diagonal = n-1+i-j;

    if (is_beginning) { // begin
      pair<int, int> prev_dp = dp_col_max.get(j);
      dp[idx] = k;
      recon[idx] = -1;

      if (prev_dp.first > 0) {
	dp[idx] = prev_dp.first + k;
	recon[idx] = prev_dp.second;
      }
    } else {
      if (continues[idx] != -1) {
	if (dp[continues[idx]] + 1 > dp[idx]) {
	  dp[idx] = dp[continues[idx]] + 1;
	  recon[idx] = continues[idx];
	}
      }

      dp_col_max.update(j, make_pair(dp[idx], idx));

      if (dp[idx] > lcskpp_length) {
	lcskpp_length = dp[idx];
	best_idx = idx;
      }
    }
  }
  
  return lcskpp_length;
}

typedef function<int (const string&, const string&, int, const vector<vector<int>>&)> solver_t;

map<string, solver_t> solvers = {
  {"dp", lcskpp_dp},
  {"better?", lcskpp_better},
  {"pavetic", lcskpp_pavetic}
};

string gen_random_string(int n, int sigma) {
  string ret;
  REP(i, n) ret.push_back((rand() % sigma) + 'A');
  return ret;
}

int main(void) {
  srand(123456789);

  map<string, double> times;

  int T = 200;
  REP(t, T) {
    int N = 1000;
    int S = 4;
    int k = rand() % 5 + 3;

    string A = gen_random_string(N, S);
    string B = gen_random_string(N, S);

    auto matches = calc_matches(A, B, k);
    int lcskpp_len = lcskpp_dp(A, B, k, matches);
    
    for (auto& solver : solvers) {
      times[solver.first] -= clock();
      int solver_lcskpp_len = solver.second(A, B, k, matches);
      times[solver.first] += clock();
      if (solver_lcskpp_len != lcskpp_len) {
        puts("BUG");
        TRACE(A _ B _ k);
        TRACE(lcskpp_len _ solver.first _ solver_lcskpp_len);
        return 0;
      }
    }
    printf("done %d/%d (N = %d, k = %d, sigma = %d)\n", t+1, T, N, k, S);
  }
  printf("\n\n");
  
  for (auto& time: times) {
    time.second /= CLOCKS_PER_SEC;
    time.second /= T;
    printf("%s -> %.4lf\n", time.first.c_str(), time.second);
  }
  
  return 0;
}

