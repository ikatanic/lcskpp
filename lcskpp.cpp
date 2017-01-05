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
  vector<pair<int, int>> matches_pairs;
  calc_matches(a, b, k, &matches_pairs);
  
  vector<vector<int>> matches(a.size());
  for (auto& p: matches_pairs) matches[p.first].push_back(p.second);
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


int lcskpp_better_mix(const string& a, const string& b, int k) {
  int n = a.size();
  int m = b.size();
  vector<pair<int, int>> matches;
  calc_matches(a, b, k, &matches);

  int n_matches = matches.size();
  vector<int> MinYPrefix(m + 1, inf);
  MinYPrefix[0] = -inf;

  int r = 0;
  int log_r = 1; // log(r+1) + 1
  
  vector<int> match_dp(matches.size());

  int ptr = 0;
  int bs_ptr = 0;
  int cont_ptr = 0;

  int matches_per_row_avg = (int)matches.size() / n;
  
  while (ptr < n_matches) {
    int i = matches[ptr].first;
    int i_ptr = ptr;
    
    while (k > 1 && cont_ptr < i_ptr && matches[cont_ptr].first < i-1) cont_ptr++;

    while (bs_ptr < n_matches && matches[bs_ptr].first < i+k) {
      int row = matches[bs_ptr].first;
      
      int row_ptr = bs_ptr;
      while (row_ptr < n_matches && matches[row_ptr].first == row) row_ptr++;
      
      if ((row_ptr - bs_ptr) * log_r * 5 < r) {
        int cur_lo = 0;
        while (bs_ptr < row_ptr) {
          int j = matches[bs_ptr].second;

          if (MinYPrefix[cur_lo] < j-k+1) {
            int lo = cur_lo+1, hi = r+1;
            while (lo < hi) {
              int mid = (lo + hi) / 2;
              if (MinYPrefix[mid] < j - k + 1)
                lo = mid + 1;
              else
                hi = mid;
            }
            cur_lo = lo;
          }
          
          match_dp[bs_ptr++] = cur_lo-1;
        }
      } else {
        int lo = 0;
        while (bs_ptr < row_ptr) {
          int j = matches[bs_ptr].second;
          while (MinYPrefix[lo] < j-k+1) lo++;
          match_dp[bs_ptr++] = lo-1;
        }
      }
    }

    while (ptr < n_matches && matches[ptr].first == i) {
      int j = matches[ptr].second;
      int l = match_dp[ptr];

      for (int s = l + k; s > l && MinYPrefix[s] > j; --s) {
        MinYPrefix[s] = j;
      }
      
      int my_dp = l+k;

      if (k > 1) {
        while (cont_ptr < i_ptr && matches[cont_ptr].second < j-1) cont_ptr++;
        if (cont_ptr < i_ptr && matches[cont_ptr].second == j-1) {
          int new_dp = match_dp[cont_ptr] + 1;
          if (new_dp > my_dp) {
            my_dp = new_dp;
            MinYPrefix[new_dp] = min(MinYPrefix[new_dp], j);
          }
          cont_ptr++;
        }
      }

      if (my_dp > r) {
        r = my_dp;
        while ((1<<log_r) < r+1) log_r++;
      }        

      match_dp[ptr++] = my_dp;
    }
  }
  
  return r;
}


int lcskpp_better_kuo_cross(const string& a, const string& b, int k) {
  int n = a.size();
  int m = b.size();
  vector<pair<int, int>> matches;
  calc_matches(a, b, k, &matches);

  int n_matches = matches.size();
  vector<int> MinYPrefix(m + 1, inf);
  MinYPrefix[0] = -inf;

  int r = 0;

  vector<int> match_dp(matches.size());

  int ptr = 0;
  int bs_ptr = 0;
  int cont_ptr = 0;
  while (ptr < n_matches) {
    int i = matches[ptr].first;
    int i_ptr = ptr;
    
    while (cont_ptr < i_ptr && matches[cont_ptr].first < i-1) cont_ptr++;

    while (bs_ptr < n_matches && matches[bs_ptr].first < i+k) {
      int lo = 0;
      int row = matches[bs_ptr].first;
      while (bs_ptr < n_matches && matches[bs_ptr].first == row) {
        int j = matches[bs_ptr].second;
        while (MinYPrefix[lo] < j-k+1) lo++;
        match_dp[bs_ptr] = lo-1;
        bs_ptr++;
      }
    }

    while (ptr < n_matches && matches[ptr].first == i) {
      int j = matches[ptr].second;
      int l = match_dp[ptr];

      // probam popravit trenutni MinYPrefix.
      for (int s = 1; s <= k; ++s) {
        MinYPrefix[l + s] = min(MinYPrefix[l + s], j);
      }
      int my_dp = l+k;

      while (cont_ptr < i_ptr && matches[cont_ptr].second < j-1) cont_ptr++;
      if (cont_ptr < i_ptr && matches[cont_ptr].second == j-1) {
        int new_dp = match_dp[cont_ptr] + 1;
        if (new_dp > my_dp) {
          my_dp = new_dp;
          MinYPrefix[new_dp] = min(MinYPrefix[new_dp], j);
        }
      }

      r = max(r, my_dp);
      match_dp[ptr] = my_dp;
      
      ptr++;
    }
  }
  
  return r;
}

int lcskpp_pavetic(const string& A, const string& B, int k) {
  int ret;
  Pavetic::lcskpp(A, B, k, &ret, NULL);
  return ret;
}

