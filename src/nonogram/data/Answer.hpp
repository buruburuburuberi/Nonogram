#pragma once

#include <nonogram/data/Array2D.hpp>
#include <nonogram/data/ClueStates.hpp>
#include <nonogram/data/Solution.hpp>

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

      using DataLocks = Array2D<bool>;

      Answer (Solution const&);

      Datum at (Slot) const;
      void fill (Slot, Datum);
      Slots dataToLock() const;
      Slots lockedData() const;
      bool isDatumLocked (Slot) const;
      void lockData (Slots, bool);

      bool isCrossed (Clues::Type, FullIndex) const;
      void cross (Clues::Type, FullIndex, bool);
      Solution::ClueIndices cluesToLock() const;
      Solution::ClueIndices lockedClues() const;
      bool isClueLocked (Clues::Type, FullIndex) const;
      void lockClues (Solution::ClueIndices, bool);

      bool canLock() const;
      bool canUnlock() const;

      void reset();

    private:
      Array2D<Datum> data_;
      DataLocks data_locks_;
      Slots data_to_lock_;

      std::map<Clues::Type, ClueStates> clues_;
    };
  }
}
