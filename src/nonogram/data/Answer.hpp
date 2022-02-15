#pragma once

#include <nonogram/data/clues/Data.hpp>
#include <nonogram/data/grid/Data.hpp>
#include <nonogram/data/ClueStates.hpp>
#include <nonogram/data/Solution.hpp>

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

      using Data = grid::Data<Datum>;
      using DataLocks = grid::Data<bool>;
      using CluesStates = std::map<Clues::Type, ClueStates>;

      Answer (Solution const&);
      Answer (Data, DataLocks, CluesStates);

      Datum at (grid::Cell) const;
      void fillData (grid::Cell, Datum);
      grid::Cells dataToLock() const;
      grid::Cells lockedData() const;
      bool isDatumLocked (grid::Cell) const;
      void lockData (grid::Cells, bool);
      void fillDataLocks (bool);

      bool isCrossed (Clues::Type, clues::FullIndex) const;
      void cross (Clues::Type, clues::FullIndex, bool);
      Solution::ClueIndices cluesToLock() const;
      Solution::ClueIndices lockedClues() const;
      bool isClueLocked (Clues::Type, clues::FullIndex) const;
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
