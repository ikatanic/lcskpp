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


int lcskpp_better_hunt(const string& a, const string& b, int k) {
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
      int j = matches[bs_ptr].second;
      int lo = lower_bound(MinYPrefix.begin(), MinYPrefix.begin() + r + 2, j-k+1) - MinYPrefix.begin();
      match_dp[bs_ptr] = lo-1;
      bs_ptr++;
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

int lcskpp_better_hunt2(const string& a, const string& b, int k) {
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
      int cur_lo = 0;
      int row = matches[bs_ptr].first;
      while (bs_ptr < n_matches && matches[bs_ptr].first == row) {
        int j = matches[bs_ptr].second;

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

        match_dp[bs_ptr] = cur_lo-1;
        bs_ptr++;
      }
    }

    int prev_l = -1;
    while (ptr < n_matches && matches[ptr].first == i) {
      int j = matches[ptr].second;
      int l = match_dp[ptr];

      // probam popravit trenutni MinYPrefix.
      if (l > prev_l) {
        for (int s = 1; s <= k; ++s) {
          MinYPrefix[l + s] = min(MinYPrefix[l + s], j);
        }
        prev_l = l;
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


int lcskpp_pavetic_ubrzan(const string& A, const string& B, int k) {
  vector<pair<int, int>> matches;
  calc_matches(A, B, k, &matches);

  int n = 0;
  for (auto& p: matches) {
    n = max(n, max(p.first, p.second));
  }
  
  // Indexed by column, first:dp value, second:index in matches.
  FenwickMax<pair<int, int> > dp_col_max(n);
  vector<int> dp(matches.size());
  vector<int> recon(matches.size());

  int best_idx = 0;
  int lcskpp_length = 0;
  auto prev = matches.begin();

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

    auto G = make_pair(event->first-1, event->second-1);
    while (*prev < G) ++prev;
    
    if (*prev == G) {
      int pidx = prev - matches.begin();
      if (dp[pidx] + 1 > dp[idx]) {
	dp[idx] = dp[pidx] + 1;
	recon[idx] = pidx;
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
  vector<pair<int, int>> matches;
  calc_matches(A, B, k, &matches);
  sort(matches.begin(), matches.end());
  
  int n = 0;
  for (auto& p: matches) {
    n = max(n, max(p.first, p.second));
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


