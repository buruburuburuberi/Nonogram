#pragma once

#include <nonogram/data/Array2D.hpp>
#include <nonogram/data/ClueStates.hpp>
#include <nonogram/data/Solution.hpp>
#include <nonogram/data/VectorOfVectors.hpp>

#include <QtCore/QDataStream>

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

      using Data = Array2D<Datum>;
      using DataLocks = Array2D<bool>;
      using CluesStates = std::map<Clues::Type, ClueStates>;

      Answer (Solution const&);
      Answer (Data, DataLocks, CluesStates);

      Datum at (Slot) const;
      void fillData (Slot, Datum);
      Slots dataToLock() const;
      Slots lockedData() const;
      bool isDatumLocked (Slot) const;
      void lockData (Slots, bool);
      void fillDataLocks (bool);

      bool isCrossed (Clues::Type, FullIndex) const;
      void cross (Clues::Type, FullIndex, bool);
      Solution::ClueIndices cluesToLock() const;
      Solution::ClueIndices lockedClues() const;
      bool isClueLocked (Clues::Type, FullIndex) const;
      void lockClues (Solution::ClueIndices, bool);
      void fillClueLocks (bool);

      bool canLock() const;
      bool canUnlock() const;
      bool isEmpty() const;

      void reset();

      // serialization
      friend QDataStream& operator>> (QDataStream&, Datum&);
      friend QDataStream& operator<< (QDataStream&, Datum const&);

      Answer (QDataStream&);
      friend QDataStream& operator<< (QDataStream&, Answer const&);

    private:
      Data data_;
      DataLocks data_locks_;

      CluesStates clue_states_;
    };
  }
}
