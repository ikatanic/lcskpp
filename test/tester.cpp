#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <functional>
#include <map>
#include <unordered_map>
#include <queue>
#include <memory>

#include "../lcskpp.h"
#include "../../lcskpp_pavetic/lcskpp/lcskpp.h"

#define FOR(i, a, b) for (int i = (a); i < (b); ++i)
#define REP(i, n) FOR(i, 0, n)
#define TRACE(x) cout << #x << " = " << x << endl
#define _ << " _ " <<

using namespace std;

const int N = 10000;
const int S = 4;

bool indicator(double p) {
  int r = rand() % 2;
  if (p >= 0.5)
    return r ? indicator(2 * p - 1) : true;
  else
    return r ? false : indicator(2 * p);
}

string gen_random_string(int n, int sigma) {
  string ret;
  REP(i, n) ret.push_back((rand() % sigma) + 'A');
  return ret;
}

string gen_with_similarity(const string &base, int sigma, double p) {
  string ret = base;
  REP(i, (int)ret.size())
    if (!indicator(p)) {
      do {
	ret[i] = (rand() % sigma) + 'A';
      } while (base[i] == ret[i]);
    }
  return ret;
}

bool check_reconstruction(vector<pair<int, int>>& recon, string& A, string& B, int r) {
  if (r != (int)recon.size()) return false;
  for (int i = 0; i < r; ++i) {
    int x = recon[i].first;
    int y = recon[i].second;
    if (x < 0 || x >= (int)A.size()) return false;
    if (y < 0 || y >= (int)B.size()) return false;

    if (i > 0 && x <= recon[i-1].first) return false;
    if (i > 0 && y <= recon[i-1].second) return false;

    if (A[x] != B[y]) return false;
  }
  return true;
}

double run(int k, double p) {
  const int IT = 10*k;

  srand(p * 100000000 + k);

  string A = gen_random_string(N, S);
  string B = gen_with_similarity(A, S, p);


  double pavetic_time = 0;
  double we_time = 0;
  int pav_len, we_len;
  
  vector<pair<int, int>> recon;
  pavetic_time = -clock();
  REP(it, IT) {
    recon.clear();
    int len;

    lcskpp(A, B, k, &len, &recon);
    pav_len = len;
  }
  pavetic_time += clock();
  if (!check_reconstruction(recon, A, B, pav_len)) {
    puts("Pavetic reconstruction is wrong.");
    TRACE(A _ B _ k);
    exit(0);
  }

  recon.clear();
  we_time = -clock();
  REP(it, IT) {
    recon.clear();
    int len;

    len = lcskpp(A, B, k, &recon);
    we_len = len;
  }
  we_time += clock();
  

  if (!check_reconstruction(recon, A, B, we_len)) {
    puts("Our reconstruction is wrong.");
    TRACE(A _ B _ k);
    exit(0);
  }
  
  if (pav_len != we_len) {
      puts("LCSK++ lengths don't match:");
      TRACE(pav_len _ we_len);
      TRACE(A _ B _ k);
      exit(0);
  }
  
  return pavetic_time / we_time;
}

enum { BLACK, RED, GREEN, YELLOW, BLUE };

void set_color(int color) {
  printf("\x1B[%dm", 30 + color);
}

void clear_color() {
  printf("\x1B[0m");
}

int main(int argc, char **argv)
{
  if (argc != 3 && argc != 5) {
    puts("Usage: ./tester [k_lo] [k_hi] {[p_lo(%)] [p_hi(%)]}");
    puts("Ex.: ./tester 4 17 40 80");
    puts("Ex.: ./tester 4 17");
    exit(0);
  }

  int k_lo = atoi(argv[1]);
  int k_hi = atoi(argv[2]);
  int p_lo = argc == 5 ? atoi(argv[3]) : 40;
  int p_hi = argc == 5 ? atoi(argv[4]) : 90;


  printf("  ");
  for (int p = p_lo; p <= p_hi; p += 10)
    printf("% 10d%%", p);
  puts("");

  FOR(k, k_lo, k_hi + 1) {
    printf("% 3d:   ", k);
    for (int p = p_lo; p <= p_hi; p += 10) {
      double factor = run(k, p / 100.0);
      int color = (factor < 3.0) ? RED : GREEN;
      set_color(color);
      printf("[%.3lf]  ", factor);
      printf("  ");
      clear_color();
    }
    puts(""); puts("");
  }

  return 0;
}
