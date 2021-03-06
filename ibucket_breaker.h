// Author: danvk@google.com (Dan Vanderkam)
//
// Attempt to eliminate whole classes of 3x3 boggle boards by computing upper
// bounds and recursively splitting cells into individual letters.

#ifndef BREAKER_H
#define BREAKER_H

#include "bucket_solver.h"
#include <string>
#include <vector>

class BreakDetails;

// A class to collect various options for the Breaker.
struct BreakOptions {
  BreakOptions() : print_progress(false), record_progress(false) {}

  bool print_progress;  // should breaking progress be printed to stdout?
  bool record_progress;  // should BreakDetails.boards_considered be filled?
};

class Breaker {
 public:
  // Does not take ownership of the BucketBuggler, though it must remain live
  // for the lifetime of the Breaker.
  Breaker(BucketSolver* solver, int best_score);

  // Attempt to break the board class.
  void Break(BreakDetails* details);

  // board is a space-separated list of letters on each cell, e.g.
  // "ab cd ef gh ij kl mn op qr"
  bool ParseBoard(const std::string& board);

  // Change the breaking options.
  void SetOptions(BreakOptions options) { options_ = options; }

  // Set the order in which cells are picked. Must be a permutation of
  // 0..(width*height - 1). Crashes if this is not the case.
  void SetPickOrder(std::vector<int>& order);

 private:
  // TODO(danvk): document these
  int PickABucket(std::vector<std::string>* splits, int level);
  bool ShedToConvergence(int level);
  void SplitBucket(int level);
  void AttackBoard(int level = 0, int num=1, int outof=1);

  BucketSolver* solver_;
  BreakDetails* details_;
  int best_score_;
  uint64_t elim_;
  uint64_t orig_reps_;

  int cells_;
  std::vector<int> order_;

  BreakOptions options_;
};

struct BreakDetails {
  int max_depth;
  uint64_t num_reps;
  double start_time;
  double elapsed;

  int sum_wins;
  int max_wins;

  std::vector<std::string> failures;

  // only filled out if options.record_progress is set.
  // boards appear in this vector in the order in which they are considered.
  std::vector<std::string> boards_considered;
};

#endif
