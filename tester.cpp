#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <functional>
#include <map>
#include <unordered_map>
#include <queue>
#include <memory>

#include "lcskpp.cpp"

#define FOR(i, a, b) for (int i = (a); i < (b); ++i)
#define REP(i, n) FOR(i, 0, n)
#define TRACE(x) cout << #x << " = " << x << endl
#define _ << " _ " <<

using namespace std;

const int N = 10000;
const int S = 4;

typedef function<int (const string&, const string&, int)> solver_t;

map<string, solver_t> solvers = {
  //{"dp", lcskpp_dp},
  {"better_hunt", lcskpp_better_hunt},
  {"better_hunt2", lcskpp_better_hunt2},
  {"better_kuo_cross", lcskpp_better_kuo_cross},
  {"pavetic", lcskpp_pavetic},
  {"pavetic_ubrzan", lcskpp_pavetic_ubrzan},
  // {"pavetic_ubrzan_no_recon", lcskpp_pavetic_ubrzan_no_recon}
};

map<string, char> codes = {
  //  {"dp", lcskpp_dp},
  {"better_hunt", 'h'},
  {"better_hunt2", 'H'},
  {"better_kuo_cross", 'k'},
  {"pavetic", 'p'},
  {"pavetic_ubrzan", 'P'},
  // {"pavetic_ubrzan_no_recon", lcskpp_pavetic_ubrzan_no_recon}
};


pair<string, double> run(int k, double p) {
  const int IT = 5;
  
  srand(p * 1000000 + k); 
  
  string A = gen_random_string(N, S);
  string B = gen_with_similarity(A, S, p);

  int lcskpp_len = lcskpp_pavetic(A, B, k);

  map<string, double> times;
  
  for (auto& solver : solvers) {
    REP(it, IT) {
      double t = -clock();
      int solver_lcskpp_len = solver.second(A, B, k);
      t += clock();
      times[solver.first] += t / IT;

      if (solver_lcskpp_len != lcskpp_len) {
        puts("BUG");
        TRACE(A _ B _ k);
        TRACE(lcskpp_len _ solver.first _ solver_lcskpp_len);
        exit(0);
      }
    }
  }

  double best_time = times["pavetic"];
  string winner = "pavetic";

  for (auto it : times) {
    if (it.second < best_time) {
      best_time = it.second;
      winner = it.first;
    }
  }

  return {winner, times["pavetic"] / best_time};
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

  puts("Znakovi:");
  for (auto it : codes)
    printf("%c : %s\n", it.second, it.first.c_str());
  puts("");

  printf("  ");
  for (int p = p_lo; p <= p_hi; p += 10)
    printf("% 10d%%", p);
  puts("");

  FOR(k, k_lo, k_hi + 1) {
    printf("% 3d:   ", k);
    for (int p = p_lo; p <= p_hi; p += 10) {
      string winner; double factor;
      tie(winner, factor) = run(k, p / 100.0);
      int color = (factor < 3.0) ? RED : GREEN;
      set_color(color);
      printf("%c[%.3lf] ", codes[winner], factor);
      printf("  ");
      clear_color();
    }
    puts(""); puts("");
  }

  return 0;
}
