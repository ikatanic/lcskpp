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


  vector<int> MinYPrefix(n + 1, inf);
  MinYPrefix[0] = -inf;

  int r = 0;

  vector<vector<int>> match_dp(n);
  REP(i, n) match_dp[i].resize(matches[i].size());
  
  REP(i, n) {
    int cont_ptr = (i == 0 ? 0 : matches[i-1].size()) - 1;
    for (int jt = (int)matches[i].size()-1; jt >= 0; --jt) {
      int j = matches[i][jt];
      int l = match_dp[i][jt];

      // probam popravit trenutni MinYPrefix.
      for (int s = 1; s <= k; ++s) {
        MinYPrefix[l + s] = min(MinYPrefix[l + s], j);
      }
      int my_dp = l+k;

      if (i > 0) {
        while (cont_ptr >= 0 && matches[i-1][cont_ptr] > j-1) cont_ptr--;
        if (cont_ptr >= 0 && matches[i-1][cont_ptr] == j-1) {
          int new_dp = match_dp[i-1][cont_ptr] + 1;
          if (new_dp > my_dp) {
            my_dp = new_dp;
            MinYPrefix[new_dp] = min(MinYPrefix[new_dp], j);
          }
        }
      }

      r = max(r, my_dp);
      match_dp[i][jt] = my_dp;
    }

    if (i + k < n) {
      // napravi bs za matcheve iz iteracije i+k
      REP(jt, (int)matches[i+k].size()) {
        int j = matches[i+k][jt]; 

        int lo = 0, hi = r+1; // hi se moze smanjit
        while (lo < hi) {
          int mid = (lo + hi) / 2;
          if (MinYPrefix[mid] < j - k + 1)
            lo = mid + 1;
          else
            hi = mid;
        }

        match_dp[i+k][jt] = lo-1;
      }
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

  int lcskpp_length = 0;
    
  for (auto event = events.begin(); 
       event != events.end(); ++event) {
    int idx = get<2>(*event) % matches.size();
    bool is_beginning = (get<2>(*event) >= (int)matches.size());
    int j = get<1>(*event);

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
      }
    }
  }
  
  return lcskpp_length;
}

int lcskpp_pavetic_ubrzan(const string& A, const string& B, int k, const vector<vector<int>>& matches_buckets) {
  vector<pair<int, int>> matches;
  for (int i = 0; i < (int)matches_buckets.size(); ++i) {
    for (int j : matches_buckets[i]) {
      matches.push_back({i, j});
    }
  }
  
  vector<tuple<int, int, int> > events;
  events.reserve(matches.size());

  int n = 0;
  for (auto it = matches.begin(); it != matches.end(); ++it) {
    int idx = it - matches.begin();
    events.push_back(make_tuple(it->first, it->second, idx)); // end
    
    n = max(n, it->first+k);
    n = max(n, it->second+k);
  }

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

  for (auto event = events.begin(), bp = events.begin(); 
       event != events.end(); ++event) {

    while (get<0>(*bp) <= get<0>(*event) - k) {
      int idx = get<2>(*bp);
      int i = get<0>(*bp);
      int j = get<1>(*bp);
      dp_col_max.update(j, make_pair(dp[idx], idx));
      ++bp;
    }
    
    int idx = get<2>(*event);
    int i = get<0>(*event);
    int j = get<1>(*event);

    dp[idx] = k;
    recon[idx] = -1;

    if (continues[idx] != -1) {
      if (dp[continues[idx]] + 1 > dp[idx]) {
	dp[idx] = dp[continues[idx]] + 1;
	recon[idx] = continues[idx];
      }
    }

    if (j >= k) {
      pair<int, int> prev_dp = dp_col_max.get(j - k);
      if (prev_dp.first > 0 && dp[idx] < prev_dp.first + k) {
	dp[idx] = prev_dp.first + k;
	recon[idx] = prev_dp.second;
      }
    }

    if (dp[idx] > lcskpp_length) {
      lcskpp_length = dp[idx];
      best_idx = idx;
    }
  }
  
  return lcskpp_length;
}

int lcskpp_pavetic_jos_ubrzan(const string& A, const string& B, int k, const vector<vector<int>>& matches_buckets) {
  vector<pair<int, int>> matches;
  for (int i = 0; i < (int)matches_buckets.size(); ++i) 
    for (int j : matches_buckets[i]) 
      matches.push_back({i, j});

  int n = 0;
  for (auto it = matches.begin(); it != matches.end(); ++it) {
    n = max(n, it->first);
    n = max(n, it->second);
  }

  // Indexed by column, first:dp value, second:index in matches.
  FenwickMax<int> dp_col_max(n);
  vector<int> dp(matches.size());
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

  int lcskpp_length = 0;

  for (auto event = matches.begin(), bp = matches.begin(); 
       event != matches.end(); ++event) {

    while (bp->first <= event->first - k) {
      int idx = bp - matches.begin(), j = bp->second;
      dp_col_max.update(j, dp[idx]);
      ++bp;
    }

    int i = event->first;
    int j = event->second;
    int idx = event - matches.begin();

    dp[idx] = k;

    if (continues[idx] != -1) 
      dp[idx] = max(dp[idx], dp[continues[idx]] + 1);

    if (j >= k) {
      int val = dp_col_max.get(j - k);
      if (val > 0)
	dp[idx] = max(dp[idx], val + k);
    }

    lcskpp_length = max(lcskpp_length, dp[idx]);
  }

  
  return lcskpp_length;
}

typedef function<int (const string&, const string&, int, const vector<vector<int>>&)> solver_t;

map<string, solver_t> solvers = {
  {"dp", lcskpp_dp},
  {"better?", lcskpp_better},
  {"pavetic", lcskpp_pavetic},
  {"pavetic_ubrzan", lcskpp_pavetic_ubrzan},
  {"pavetic_jos_ubrzan", lcskpp_pavetic_jos_ubrzan}
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
    printf("%s -> %.6lf\n", time.first.c_str(), time.second);
  }
  
  return 0;
}

