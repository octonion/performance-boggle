// Author: danvk@google.com (Dan Vanderkam)
//
// Attempt to eliminate whole classes of 3x3 boggle boards by computing upper
// bounds and recursively splitting cells into individual letters.

#ifndef PAPER_BREAKER_H
#define PAPER_BREAKER_H

#include "ibuckets.h"

namespace paper {

class BreakDetails;

class Breaker {
 public:
  // Does not take ownership of the BucketBuggler, though it must remain live
  // for the lifetime of the Breaker.
  // Will detect all boards whose scores are _strictly_ greater than best_score.
  Breaker(BucketBoggler* bb, int best_score);

  // Attempt to break the board class.
  // The BucketBoggler is modified by this call.
  void Break(BreakDetails* details);

  // board is a space-separated list of letters on each cell, e.g.
  // "ab cd ef gh ij kl mn op qr"
  bool ParseBoard(const std::string& board);

  // Construct a board class by setting each cell to one of the buckets in
  // classes. If classes contains N letter classes, then indices 0 to N^9-1 are
  // all valid.
  // Returns false if the index is invalid.
  bool FromId(const std::vector<std::string>& classes, uint64_t idx);

  // Returns true if this is the least index corresponding to a symmetric
  // board.
  static bool IsCanonical(int num_classes, uint64_t idx);

  // Given a board array, calculate an ID for it.
  typedef int BdArray[3][3];
  static uint64_t BoardId(const BdArray& bd, int num_classes);

  // Should progress information be displayed? Default is true.
  void SetDisplayDebugOutput(bool display) { debug_ = display; }

 private:
  // TODO(danvk): document these
  int PickABucket(std::vector<std::string>* splits, int level);
  void SplitBucket(int level);
  void AttackBoard(int level = 0, int num=1, int outof=1);

  BucketBoggler* bb_;
  BreakDetails* details_;
  int best_score_;
  uint64_t elim_;
  uint64_t orig_reps_;

  bool debug_;
};

struct BreakDetails {
  int max_depth;
  uint64_t num_reps;
  double start_time;
  double elapsed;

  std::vector<std::string> failures;
};

}  // namespace paper

#endif
