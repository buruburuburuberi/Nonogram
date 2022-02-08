#pragma once

#include <nonogram/data/Array2D.hpp>
#include <nonogram/data/ClueStates.hpp>
#include <nonogram/data/Solution.hpp>

#include <cstddef>
#include <map>

namespace nonogram
{
  namespace file
  {
    class Puzzles;
  }

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

      using Data = Array2D<Datum>;
      using DataLocks = Array2D<bool>;
      using CluesStates = std::map<Clues::Type, ClueStates>;

      Answer (Solution const&);
      Answer (Data, DataLocks, CluesStates);

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
      bool isEmpty() const;

      void reset();

    private:
      Data data_;
      DataLocks data_locks_;
      Slots data_to_lock_;

      CluesStates clue_states_;

      friend class file::Puzzles;
    };
  }
}
