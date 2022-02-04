#pragma once

#include <nonogram/data/Array2D.hpp>
#include <nonogram/data/Solution.hpp>

#include <set>

namespace nonogram
{
  namespace data
  {
    using ClueState = bool;

    class ClueStates
    {
    public:
      ClueStates (Solution const&, Solution::ClueType);

      bool isCrossed (Slot) const;
      void cross (Slot, ClueState);
      Slots toLock() const;
      Slots locked() const;
      bool isLocked (Slot) const;
      void lock (Slots, bool);

      bool canLock() const;
      bool canUnlock() const;

      void reset();

    private:
      Solution::ClueType type_;
      Array2D<ClueState> data_;
      std::vector<int> number_of_elements_;
      Array2D<ClueState> locks_;
      Slots to_lock_;
    };
  }
}
