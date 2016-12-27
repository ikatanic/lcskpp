// razni lcs algoritmi

// notacija:
// stringovi a, b
// n = |a|, m = |b|
// r = |lcs(a, b)|
// M = |{(i, j) | a[i] == b[j]}|

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <functional>
#include <map>

using namespace std;

#define FOR(i, a, b) for (int i = (a); i < (b); ++i)
#define REP(i, n) FOR(i, 0, n)
#define TRACE(x) cout << #x << " = " << x << endl
#define _ << " _ " <<

typedef long long llint;


// obican dp: O(nm)
int lcs_dp(const string& a, const string& b) {
  int n = a.size();
  int m = b.size();
  vector<vector<int>> f(n + 1, vector<int>(m + 1, 0));
  REP(i, n+1) REP(j, m+1) {
    if (i < n) f[i+1][j] = max(f[i+1][j], f[i][j]);
    if (j < m) f[i][j+1] = max(f[i][j+1], f[i][j]);
    if (i < n && j < m && a[i] == b[j]) f[i+1][j+1] = max(f[i+1][j+1], f[i][j] + 1);
  }
  return f[n][m];
}



// Hunt and Szymanski [22]: O(M log m)
int lcs_hunt(const string& a, const string& b) {
  int n = a.size();
  int m = b.size();
  
  int sigma = 0;
  REP(i, n) sigma = max(sigma, a[i] - 'A' + 1);
  REP(i, m) sigma = max(sigma, b[i] - 'A' + 1);
  
  vector<vector<int>> a_char_idxs(sigma);
  vector<vector<int>> b_char_idxs(sigma);
  
  REP(i, n) a_char_idxs[a[i] - 'A'].push_back(i); 
  REP(i, m) b_char_idxs[b[i] - 'A'].push_back(i);
  
  vector<int> MinYPrefix(n + 1, m + 1);

  MinYPrefix[0] = 0;
  int r = 0;
  
  REP(i, n) {
    int char_idx = a[i] - 'A';

    // for each match (from right to left) find j such that it is in interval [MinYPrefix[j], MinYPrefix[j+1]>
    for (auto b_match_iter = b_char_idxs[char_idx].rbegin();
         b_match_iter != b_char_idxs[char_idx].rend();
         ++b_match_iter) {
      
      int j = upper_bound(MinYPrefix.begin(), MinYPrefix.begin()+r+2, *b_match_iter) - MinYPrefix.begin() - 1;
      MinYPrefix[j+1] = *b_match_iter+1;
      if (j == r) r++;
    }
  }
  return r;
}



// Kua and Cross [26]: O(M + nr)
int lcs_kua(const string& a, const string& b) {
  int n = a.size();
  int m = b.size();
  
  int sigma = 0;
  REP(i, n) sigma = max(sigma, a[i] - 'A' + 1);
  REP(i, m) sigma = max(sigma, b[i] - 'A' + 1);
  
  vector<vector<int>> a_char_idxs(sigma);
  vector<vector<int>> b_char_idxs(sigma);
  
  REP(i, n) a_char_idxs[a[i] - 'A'].push_back(i); 
  REP(i, m) b_char_idxs[b[i] - 'A'].push_back(i);
  
  vector<int> MinYPrefix(n + 1, m + 1);

  MinYPrefix[0] = 0;

  REP(i, n) {
    int char_idx = a[i] - 'A';

    // for each match, find j such that it is in interval [MinYPrefix[j], MinYPrefix[j+1]>
    // sweep over j
    int j = 0;
    int prev_lo = 0; // this is MinYPrefix[j] from last iteration of i
    for (auto l : b_char_idxs[char_idx]) {
      if (l >= prev_lo) {
        j++;
        while (l >= MinYPrefix[j]) j++;
        prev_lo = MinYPrefix[j];
        MinYPrefix[j] = l+1;
      }
    }
  }
  
  int r = 0;
  while (r < n && MinYPrefix[r+1] != m+1) r++;
  return r;
}




typedef function<int (const string&, const string&)> solver_t;

map<string, solver_t> solvers = {
  {"dp", lcs_dp},
  {"hunt", lcs_hunt},
  {"kua", lcs_kua},
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

    int lcs_len = lcs_dp(A, B);
    
    for (auto& solver : solvers) {
      times[solver.first] -= clock();
      int solver_lcs_len = solver.second(A, B);
      times[solver.first] += clock();
      if (solver_lcs_len != lcs_len) {
        puts("BUG");
        TRACE(lcs_len _ solver.first _ solver_lcs_len);
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
