// Use Simulated Annealing to find a good Boggle board.
// 3x3/anneal.cc is just like anneal.cc, but uses 3x3 boards.
// Best I've found so far: streaedlp = 545 points.

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "gflags/gflags.h"
#include "mtrandom/randomc.h"
#include "trie.h"
#include "3x3/boggler.h"

DEFINE_double(cool_t0, 100.0, "Initial temperature");
DEFINE_double(cool_k, 0.05, "Cooling constant (controls pace of cooling)");
DEFINE_double(swap_ratio, 1.0, "Ratio of swaps to letter changes");
DEFINE_double(mutation_p, 0.75, "Probability of just one change/mutation");
DEFINE_int32(max_stall, 1000, "Number of generations before a change to exit");
DEFINE_int32(rand_seed, -1, "Random seed (-1 means use time + pid)");

DEFINE_bool(print_scores, false, "Print the score of each board considered");
DEFINE_bool(print_stats, false, "Print statistics after the annealing run");
DEFINE_bool(print_transitions, true, "Print each acceptedtransition");
DEFINE_bool(print_params, true, "Print parameters before beginning run");

DEFINE_string(dictionary, "words", "Path to dictionary of words");

static const int NumSquares = 9;

typedef TRandomMersenne Random;

// Should we transition between boards with these two scores?
bool AcceptTransition(int cur_score, int new_score, double T, Random* rand) {
  if (new_score > cur_score)
    return true;
  double p = exp((new_score - cur_score)/T);
  return (rand->Random() < p);
}

// "Temperature" after n iterations
double Temperature(int n) {
  return FLAGS_cool_t0 * exp(-FLAGS_cool_k * n);
}

// Mutate a board
static int total_swaps = 0;
static int total_changes = 0;
static int total_mutations = 0;
static int total_mutate_calls = 0;
void Mutate(char* bd, Random* rand) {
  total_mutate_calls += 1;
  do {
    total_mutations += 1;
    if ((1.0 + FLAGS_swap_ratio) * rand->Random() > 1.0) {
      // swap two cells
      total_swaps += 1;
      int a, b;
      do {
        int pair = rand->IRandom(0, NumSquares * NumSquares - 1);
        a = pair / NumSquares;
        b = pair % NumSquares;
      } while (bd[a] == bd[b]);
      char tmp = bd[a];
      bd[a] = bd[b];
      bd[b] = tmp;
    } else {
      // change a cell
      total_changes += 1;
      int cell, letter;
      do {
        int pair = rand->IRandom(0, 26 * NumSquares - 1);
        cell = pair / 26;
        letter = 'a' + pair % 26;
      } while (bd[cell] == letter);
      bd[cell] = letter;
    }
  } while (rand->Random() > FLAGS_mutation_p);
}

// A random initial board. Does not set a trailing null char!
void InitialBoard(char* bd, Random* rand) {
  for (int i = 0; i < NumSquares; i++) {
    bd[i] = rand->IRandom('a', 'z');
  }
}

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  if (FLAGS_rand_seed == -1) {
    FLAGS_rand_seed = time(NULL) + getpid();
  }

  if (FLAGS_print_params) {
    printf("Annealing parameters:\n");
    printf(" cool_t0: %lf\n", FLAGS_cool_t0);
    printf(" cool_k: %lf\n", FLAGS_cool_k);
    printf(" swap_ratio: %lf\n", FLAGS_swap_ratio);
    printf(" mutation_p: %lf\n", FLAGS_mutation_p);
    printf(" max_stall: %d\n", FLAGS_max_stall);
    printf(" rand_seed: %d\n", FLAGS_rand_seed);
    printf(" dictionary: %s\n", FLAGS_dictionary.c_str());
  }

  Trie* t = Boggler::DictionaryFromFile(FLAGS_dictionary.c_str());
  Boggler b(t);

  // TODO(danvk): sanity-check parameters
  Random r(FLAGS_rand_seed);
  char bd[1 + NumSquares] = "";
  char last[1 + NumSquares] = "abcdefghi";
  InitialBoard(last, &r);
  int last_accept = 0;
  int last_score = -1;
  int total_transitions = 0;
  for (int n = 0; n < last_accept + FLAGS_max_stall; n++) {
    memcpy(bd, last, sizeof(last));
    Mutate(bd, &r);
    int score = b.Score(bd);
    if (FLAGS_print_scores)
      printf("%d\t%d\n", n, score);
    double T = Temperature(n);
    if (AcceptTransition(last_score, score, T, &r)) {
      total_transitions += 1;
      last_accept = n;
      last_score = score;
      memcpy(last, bd, sizeof(bd));
      if (FLAGS_print_transitions)
        printf("%5d T=%3.8lf accepting '%s' (%d)\n", n, T, last, last_score);
    }
  }

  printf(" final board: %s\n", last);
  printf(" final score: %d\n", last_score);

  if (FLAGS_print_stats) {
    printf(" transitions: %d\n", total_transitions);
    printf("       swaps: %d\n", total_swaps);
    printf("     changes: %d\n", total_changes);
    printf("   mutations: %d\n", total_mutations);
    printf("mutate_calls: %d\n", total_mutate_calls);
  }
}