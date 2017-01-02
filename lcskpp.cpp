// lcskpp algoritmi

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <functional>
#include <map>
#include <queue>
#include "utils.h"
#include "pavetic.h"

using namespace std;

#define FOR(i, a, b) for (int i = (a); i < (b); ++i)
#define REP(i, n) FOR(i, 0, n)
#define TRACE(x) cout << #x << " = " << x << endl
#define _ << " _ " <<

typedef long long llint;

const int inf = 1e9;

// obican dp: O(nmk)
int lcskpp_dp(const string& a, const string& b, int k) {
  vector<vector<int>> matches = calc_matches(a, b, k);
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


int lcskpp_better_hunt(const string& a, const string& b, int k) {
  vector<vector<int>> matches = calc_matches(a, b, k);
  int n = a.size();
  //  int m = b.size();


  vector<int> MinYPrefix(n + 1, inf);
  MinYPrefix[0] = -inf;

  int r = 0;

  vector<vector<int>> match_dp(n);
  REP(i, n) match_dp[i].resize(matches[i].size());
  
  REP(i, n) {
    int cont_ptr = 0;
    REP(jt, (int)matches[i].size()) {
      int j = matches[i][jt];
      int l = match_dp[i][jt];

      // probam popravit trenutni MinYPrefix.
      for (int s = 1; s <= k; ++s) {
        MinYPrefix[l + s] = min(MinYPrefix[l + s], j);
      }
      int my_dp = l+k;

      if (i > 0) {
        while (cont_ptr < (int)matches[i-1].size() && matches[i-1][cont_ptr] < j-1) cont_ptr++;
        if (cont_ptr < (int)matches[i-1].size() && matches[i-1][cont_ptr] == j-1) {
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

        // int lo = 0, hi = r+1; // hi se moze smanjit
        // while (lo < hi) {
        //   int mid = (lo + hi) / 2;
        //   if (MinYPrefix[mid] < j - k + 1)
        //     lo = mid + 1;
        //   else
        //     hi = mid;
        // }
        int lo = lower_bound(MinYPrefix.begin(), MinYPrefix.begin() + r + 2, j-k+1) - MinYPrefix.begin();
        match_dp[i+k][jt] = lo-1;
      }
    }
  }
  
  return r;
}

int lcskpp_better_hunt2(const string& a, const string& b, int k) {
  vector<vector<int>> matches = calc_matches(a, b, k);
  int n = a.size();
  //  int m = b.size();


  vector<int> MinYPrefix(n + 1, inf);
  MinYPrefix[0] = -inf;

  int r = 0;

  vector<vector<int>> match_dp(n);
  REP(i, n) match_dp[i].resize(matches[i].size());
  
  REP(i, n) {
    int cont_ptr = 0;
    int prev_l = -1;
    REP(jt, (int)matches[i].size()) {
      int j = matches[i][jt];
      int l = match_dp[i][jt];

      // probam popravit trenutni MinYPrefix.
      // samo ako sam prvi s tim l
      if (l > prev_l) {
        prev_l = l;
        for (int s = 1; s <= k; ++s) {
          MinYPrefix[l + s] = min(MinYPrefix[l + s], j);
        }
      }
      int my_dp = l+k;

      if (i > 0) {
        while (cont_ptr < (int)matches[i-1].size() && matches[i-1][cont_ptr] < j-1) cont_ptr++;
        if (cont_ptr < (int)matches[i-1].size() && matches[i-1][cont_ptr] == j-1) {
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
      int cur_lo = 0;
      REP(jt, (int)matches[i+k].size()) {
        int j = matches[i+k][jt]; 

        if (MinYPrefix[cur_lo] < j-k+1) {
          int lo = cur_lo, hi = r+1;
          while (lo < hi) {
            int mid = (lo + hi) / 2;
            if (MinYPrefix[mid] < j - k + 1)
              lo = mid + 1;
            else
              hi = mid;
          }
          cur_lo = lo;
        }

        match_dp[i+k][jt] = cur_lo-1;
      }
    }
  }
  
  return r;
}

int lcskpp_better_kuo_cross(const string& a, const string& b, int k) {
  vector<vector<int>> matches = calc_matches(a, b, k);
  int n = a.size();
  //  int m = b.size();


  vector<int> MinYPrefix(n + 1, inf);
  MinYPrefix[0] = -inf;

  int r = 0;

  vector<vector<int>> match_dp(n);
  REP(i, n) match_dp[i].resize(matches[i].size());
  
  REP(i, n) {
    int cont_ptr = 0;
    REP(jt, (int)matches[i].size()) {
      int j = matches[i][jt];
      int l = match_dp[i][jt];

      // probam popravit trenutni MinYPrefix.
      for (int s = 1; s <= k; ++s) {
        MinYPrefix[l + s] = min(MinYPrefix[l + s], j);
      }
      int my_dp = l+k;

      if (i > 0) {
        while (cont_ptr < (int)matches[i-1].size() && matches[i-1][cont_ptr] < j-1) cont_ptr++;
        if (cont_ptr < (int)matches[i-1].size() && matches[i-1][cont_ptr] == j-1) {
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
      int lo = 0;
      REP(jt, (int)matches[i+k].size()) {
        int j = matches[i+k][jt]; 
        while (MinYPrefix[lo] < j-k+1) lo++;
        match_dp[i+k][jt] = lo-1;
      }
    }
  }
  
  return r;
}

int lcskpp_pavetic(const string& A, const string& B, int k) {
  int ret;
  Pavetic::lcskpp(A, B, k, &ret, NULL);
  return ret;
}


int lcskpp_pavetic_ubrzan(const string& A, const string& B, int k) {
  vector<vector<int>> matches_buckets = calc_matches2(A, B, k);
  vector<pair<int, int>> matches;

  int n = 0;
  for (int i = 0; i < (int)matches_buckets.size(); ++i) {
    for (int j : matches_buckets[i]) {
      matches.push_back({i, j});
      n = max(n, i);
      n = max(n, j);
    }
  }
  
  // Indexed by column, first:dp value, second:index in matches.
  FenwickMax<pair<int, int> > dp_col_max(n);
  vector<int> dp(matches.size());
  vector<int> recon(matches.size());
  vector<int> continues(matches.size(), -1);

  if (k > 1) {
    auto prev = matches.begin();
    for (auto curr = matches.begin(); 
         curr != matches.end(); ++curr) {
      auto G = make_pair(curr->first-1, curr->second-1);
      while (*prev < G) ++prev;
      if (*prev == G) {
	continues[curr-matches.begin()] = prev-matches.begin();
      }
    }
  }

  int best_idx = 0;
  int lcskpp_length = 0;

  for (auto event = matches.begin(), bp = matches.begin(); 
       event != matches.end(); ++event) {

    while (bp->first <= event->first - k) {
      int idx = bp - matches.begin(), j = bp->second;
      dp_col_max.update(j, make_pair(dp[idx], idx));
      ++bp;
    }

    int i = event->first;
    int j = event->second;
    int idx = event - matches.begin();

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

int lcskpp_pavetic_ubrzan_no_recon(const string& A, const string& B, int k) {
  vector<vector<int>> matches_buckets = calc_matches2(A, B, k);
  vector<pair<int, int>> matches;

  int n = 0;
  for (int i = 0; i < (int)matches_buckets.size(); ++i) {
    for (int j : matches_buckets[i]) {
      matches.push_back({i, j});
      n = max(n, i);
      n = max(n, j);
    }
  }

  // Indexed by column, first:dp value, second:index in matches.
  FenwickMax<int> dp_col_max(n);
  vector<int> dp(matches.size());
  vector<int> continues(matches.size(), -1);

  if (k > 1) {
    auto prev = matches.begin();
    for (auto curr = matches.begin(); 
         curr != matches.end(); ++curr) {
      auto G = make_pair(curr->first-1, curr->second-1);
      while (*prev < G) ++prev;
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


