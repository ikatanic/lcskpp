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

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
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


int lcskpp_better_mix(const string& a, const string& b, int k, vector<pair<int, int>>* reconstruction) {
  int m = b.size();
  vector<pair<int, int>> matches;
  calc_matches(a, b, k, &matches);

  int n_matches = matches.size();
  vector<pair<int, int>> MinYPrefix(m + 1, {inf, -1});
  MinYPrefix[0].first = -inf;

  int r = 0;
  int log_r = 1; // log(r+1) + 1
  
  vector<int> match_dp(matches.size());
  vector<int> recon(matches.size());
  
  int ptr = 0;
  int bs_ptr = 0;
  int cont_ptr = 0;
  int last_idx = -1;
  
  while (ptr < n_matches) {
    int i = matches[ptr].first;
    int i_ptr = ptr;
    
    while (k > 1 && cont_ptr < i_ptr && matches[cont_ptr].first < i-1) cont_ptr++;

    while (bs_ptr < n_matches && matches[bs_ptr].first < i+k) {
      int row = matches[bs_ptr].first;
      
      int row_ptr = bs_ptr;
      while (row_ptr < n_matches && matches[row_ptr].first == row) row_ptr++;
      
      if ((row_ptr - bs_ptr) * log_r * 6 < r) {
        int cur_lo = 0;
        while (bs_ptr < row_ptr) {
          int j = matches[bs_ptr].second;

          if (MinYPrefix[cur_lo].first < j-k+1) {
            int lo = cur_lo+1, hi = r+1;
            while (lo < hi) {
              int mid = (lo + hi) / 2;
              if (MinYPrefix[mid].first < j - k + 1)
                lo = mid + 1;
              else
                hi = mid;
            }
            cur_lo = lo;
          }

          recon[bs_ptr] = MinYPrefix[cur_lo-1].second;
          match_dp[bs_ptr++] = cur_lo-1;
        }
      } else {
        int lo = 0;
        while (bs_ptr < row_ptr) {
          int j = matches[bs_ptr].second;
          while (MinYPrefix[lo].first < j-k+1) lo++;
          recon[bs_ptr] = MinYPrefix[lo-1].second;
          match_dp[bs_ptr++] = lo-1;
        }
      }
    }

    while (ptr < n_matches && matches[ptr].first == i) {
      int j = matches[ptr].second;
      int l = match_dp[ptr];

      for (int s = l + k; s > l && MinYPrefix[s].first > j; --s) {
        MinYPrefix[s] = {j, ptr};
      }
      
      int my_dp = l+k;

      if (k > 1) {
        while (cont_ptr < i_ptr && matches[cont_ptr].second < j-1) cont_ptr++;
        if (cont_ptr < i_ptr && matches[cont_ptr].second == j-1) {
          int new_dp = match_dp[cont_ptr] + 1;
          if (new_dp > my_dp) {
            my_dp = new_dp;
            recon[ptr] = cont_ptr;
            MinYPrefix[my_dp] = min(MinYPrefix[my_dp], {j, ptr});
          }
          cont_ptr++;
        }
      }

      if (my_dp > r) {
        r = my_dp;
        last_idx = ptr;
        while ((1<<log_r) < r+1) log_r++;
      }        

      match_dp[ptr++] = my_dp;
    }
  }

  if (reconstruction) {
    reconstruction->clear();
    
    int i = last_idx;
    while (i != -1) {
      assert(0 <= i && i < matches.size());

      if (recon[i] != -1 &&
          matches[recon[i]].first+1 == matches[i].first &&
          matches[recon[i]].second+1 == matches[i].second) {
        // continuation
        reconstruction->push_back({matches[i].first, matches[i].second});
      } else {
        // non-overlapping match
        assert(recon[i] == -1 || (matches[recon[i]].first + k <= matches[i].first &&
                                  matches[recon[i]].second + k <= matches[i].second));
        for (int j = 0; j < k; ++j) {
          reconstruction->push_back({matches[i].first-j, matches[i].second-j});
        }
      }

      i = recon[i];
    }
    reverse(reconstruction->begin(), reconstruction->end());
  }
  
  return r;
}

int lcskpp_pavetic(const string& A, const string& B, int k, vector<pair<int, int>>* v) {
  int ret;
  Pavetic::lcskpp(A, B, k, &ret, v);
  return ret;
}

