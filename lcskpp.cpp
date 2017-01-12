// Copyright 2017: I. Katanic, G. Matula

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "utils.h"

using namespace std;


int lcskpp(const string& a, const string& b, int k,
          vector<pair<int, int>>* reconstruction) {
  int m = b.size();
  vector<pair<int, int>> matches;
  calc_matches(a, b, k, &matches);

  int n_matches = matches.size();
  vector<pair<int, int>> MinYPrefix(m + 1, {m+1, -1});
  MinYPrefix[0].first = -1;

  int r = 0;  // current lcsk++ length
  int log_r = 1;  // log(r+1) + 1

  vector<int> match_dp(matches.size());
  vector<int> recon(matches.size());

  int ptr = 0;
  int query_ptr = 0;
  int cont_ptr = 0;
  int last_idx = -1;

  // We process matches row by row (while loop).
  // When at row i, we also handle queries on MinYPrefix for matches
  // in row i+k-1 (query_ptr).
  // cont_ptr points to ptr's possible continuation (applicable only to k > 1).
  while (ptr < n_matches) {
    int i = matches[ptr].first;  // current row index
    int i_row_ptr = ptr;


    while (query_ptr < n_matches && matches[query_ptr].first < i+k) {
      int query_row_i = matches[query_ptr].first;

      // count matches in row query_row_i
      int query_row_end = query_ptr;
      while (query_row_end < n_matches &&
             matches[query_row_end].first == query_row_i) query_row_end++;
      int row_count = query_row_end - query_ptr;

      // decide between binary search for each match (Hunt and Szymanski) and
      // sweep through MinYPrefix (Kuo and Cross).
      if (row_count * log_r * 6 < r) {
        int last_l = 0;
        while (query_ptr < query_row_end) {
          int j = matches[query_ptr].second;

          if (MinYPrefix[last_l].first < j-k+1) {
            int lo = last_l+1, hi = r+1;
            while (lo < hi) {
              int mid = (lo + hi) / 2;
              if (MinYPrefix[mid].first < j - k + 1)
                lo = mid + 1;
              else
                hi = mid;
            }
            last_l = lo;
          }

          recon[query_ptr] = MinYPrefix[last_l-1].second;
          match_dp[query_ptr++] = last_l-1+k;
        }
      } else {
        int l = 0;
        while (query_ptr < query_row_end) {
          int j = matches[query_ptr].second;
          while (MinYPrefix[l].first < j-k+1) l++;
          recon[query_ptr] = MinYPrefix[l-1].second;
          match_dp[query_ptr++] = l-1+k;
        }
      }
    }

    if (k > 1) {
      while (cont_ptr < i_row_ptr && matches[cont_ptr].first < i-1) cont_ptr++;
    }

    // now the main loop through matches in row i, finishing off their
    // dp calculation and doing updates to MinYPrefix
    while (ptr < n_matches && matches[ptr].first == i) {
      int j = matches[ptr].second;
      int& ptr_dp = match_dp[ptr];

      // update MinYPrefix
      for (int s = ptr_dp; s > ptr_dp-k && MinYPrefix[s].first > j; --s) {
        MinYPrefix[s] = {j, ptr};
      }

      // Try continuation
      if (k > 1) {
        while (cont_ptr < i_row_ptr && matches[cont_ptr].second < j-1)
          cont_ptr++;
        if (cont_ptr < i_row_ptr && matches[cont_ptr].second == j-1 &&
            match_dp[cont_ptr] + 1 > ptr_dp) {
                ptr_dp = match_dp[cont_ptr] + 1;
                recon[ptr] = cont_ptr;
                MinYPrefix[ptr_dp] = min(MinYPrefix[ptr_dp], {j, ptr});
              }
      }

      if (ptr_dp > r) {
        r = ptr_dp;
        last_idx = ptr;
        while ((1 << log_r) < r+1) log_r++;
      }

      ptr++;
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
        assert(recon[i] == -1 ||
          (matches[recon[i]].first + k <= matches[i].first &&
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
