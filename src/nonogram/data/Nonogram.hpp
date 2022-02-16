#pragma once

#include <nonogram/data/Answer.hpp>
#include <nonogram/data/Solution.hpp>
#include <nonogram/data/clues/FullIndex.hpp>
#include <nonogram/data/clues/FullIndices.hpp>
#include <nonogram/data/clues/MainIndex.hpp>
#include <nonogram/data/clues/MinorIndex.hpp>
#include <nonogram/data/grid/Cell.hpp>
#include <nonogram/data/grid/Cells.hpp>
#include <nonogram/data/grid/Column.hpp>
#include <nonogram/data/grid/Row.hpp>
#include <nonogram/util/hard_qstring_typedef.hpp>

#include <QtCore/QString>

#include <algorithm>
#include <cstddef>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>

namespace nonogram
{
  namespace data
  {
    class Nonogram
    {
    public:
      HARD_QSTRING_TYPEDEF (Pack);
      HARD_QSTRING_TYPEDEF (Puzzle);

      static Pack internalPack();

      using Packs = std::set<Pack>;
      using Puzzles = std::set<Puzzle>;

      struct ID
      {
        ID (Pack, Puzzle);
        ID (QString);

        Pack pack;
        Puzzle puzzle;

        bool internal() const;

        QString toString() const;

        bool operator== (ID const&) const;
        bool operator< (ID const&) const;
      };

      Nonogram (ID, Solution);
      Nonogram (ID, Solution, Answer);

      ID id() const;
      Answer::Data const& data() const;
      grid::Row dataRows() const;
      grid::Column dataColumns() const;
      Solution::State solution (grid::Cell) const;
      Answer::Datum answer (grid::Cell) const;
      void fillData (grid::Cell, Answer::Datum);
      grid::Cells dataToLock() const;
      grid::Cells lockedData() const;
      bool isDatumLocked (grid::Cell) const;
      void lockData (grid::Cells, bool);
      void fillDataLocks (bool);

      clues::MinorIndex maxNumberOfClues (Clues::Type type) const;
      clues::MainIndex clueMainSize (Clues::Type) const;
      clues::MinorIndex clueMinorSize (Clues::Type, clues::MainIndex) const;
      Clues::Value clue (Clues::Type, clues::FullIndex) const;
      ClueState isCrossed (Clues::Type, clues::FullIndex) const;
      void cross (Clues::Type, clues::FullIndex, ClueState);
      Solution::ClueIndices cluesToLock() const;
      Solution::ClueIndices lockedClues() const;
      bool isClueLocked (Clues::Type, clues::FullIndex) const;
      void lockClues (Solution::ClueIndices, bool);
      void fillClueLocks (bool);

      bool canLock() const;
      bool canUnlock() const;
      bool isEmpty() const;

      bool isMistake (grid::Cell) const;
      std::optional<grid::Cell> findFirstMistake() const;
      bool isSolved() const;
      void resetAnswer();

      Answer const& answer() const;

    private:
      ID id_;
      Solution solution_;
      Answer answer_;
    };
  }
}
