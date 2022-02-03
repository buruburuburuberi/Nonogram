#pragma once

#include <nonogram/data/Array2D.hpp>
#include <nonogram/data/Solution.hpp>

#include <QtCore/QSize>

#include <cstddef>
#include <map>

namespace nonogram
{
  namespace data
  {
    class Answer
    {
    public:
      enum class Datum
      { Empty = 0
      , Filled = 1
      , Crossed = 2
      , FillMark = 3
      , CrossMark = 4
      };

      using ClueState = bool;
      using ClueStates = std::map<Solution::ClueType, Array2D<ClueState>>;

      using DataLocks = Array2D<bool>;
      using ClueLocks = std::map<Solution::ClueType, Array2D<bool>>;

      Answer (Solution const&);

      Datum at (Slot) const;
      void fill (Slot, Datum);
      Slots dataToLock() const;
      Slots lockedData() const;
      bool isDatumLocked (Slot) const;
      void lockData (Slots, bool);

      bool isCrossed (Solution::ClueType, Slot) const;
      void cross (Solution::ClueType, Slot, ClueState);
      Solution::ClueSlots cluesToLock() const;
      Solution::ClueSlots lockedClues() const;
      bool isClueLocked (Solution::ClueType, Slot) const;
      void lockClues (Solution::ClueSlots, bool);

      bool canLock() const;
      bool canUnlock() const;

      void reset();

    private:
      Array2D<Datum> data_;
      DataLocks data_locks_;
      ClueStates clue_states_;
      ClueLocks clue_locks_;

      Slots data_to_lock_;
      Solution::ClueSlots clues_to_lock_;
    };
  }
}
