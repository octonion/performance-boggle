#ifndef BREAKING_TREE_H
#define BREAKING_TREE_H

#include <vector>

class BreakingNode {
 public:
  BreakingNode() {}
  ~BreakingNode() {
    for (int i = 0; i < children.size(); i++) {
      if (children[i]) delete children[i];
    }
  }

  // This may not be strictly necessary to keep around, weird as it seems.
  char letter;  // set to '\0' for a 'choice' node or 'R' for root.

  // These might be the various options on a cell or the various directions.
  std::vector<BreakingNode*> children;

  // cached computation across all children
  int bound;

  // points contributed by _this_ node.
  int points;

  // bit mask of which possibilities eventually get hit by children.
  // can be used to short-circuit recomputation of bounds.
  std::vector<bool> child_possibilities;

  int RecomputeScore();

  BreakingNode* Prune();
};

#endif
