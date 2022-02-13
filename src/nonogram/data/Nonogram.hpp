#pragma once

#include <nonogram/data/Answer.hpp>
#include <nonogram/data/Array2D.hpp>
#include <nonogram/data/Solution.hpp>
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
  namespace file
  {
    class Puzzles;
  }

  namespace data
  {
    class Nonogram
    {
    public:
      HARD_QSTRING_TYPEDEF (Pack);
      HARD_QSTRING_TYPEDEF (Puzzle);

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
      Row dataRows() const;
      Column dataColumns() const;
      Solution::State solution (Slot) const;
      Answer::Datum answer (Slot) const;
      void fillData (Slot, Answer::Datum);
      Slots dataToLock() const;
      Slots lockedData() const;
      bool isDatumLocked (Slot) const;
      void lockData (Slots, bool);
      void fillDataLocks (bool);

      MinorIndex maxNumberOfClues (Clues::Type type) const;
      MainIndex clueMainSize (Clues::Type) const;
      MinorIndex clueMinorSize (Clues::Type, MainIndex) const;
      Clues::Value clue (Clues::Type, FullIndex) const;
      ClueState isCrossed (Clues::Type, FullIndex) const;
      void cross (Clues::Type, FullIndex, ClueState);
      Solution::ClueIndices cluesToLock() const;
      Solution::ClueIndices lockedClues() const;
      bool isClueLocked (Clues::Type, FullIndex) const;
      void lockClues (Solution::ClueIndices, bool);
      void fillClueLocks (bool);

      bool canLock() const;
      bool canUnlock() const;
      bool isEmpty() const;

      bool isMistake (Slot) const;
      std::optional<Slot> findFirstMistake() const;
      bool isSolved() const;
      void resetAnswer();

    private:
      ID id_;
      Solution solution_;
      Answer answer_;

      friend class file::Puzzles;
    };
  }
}
