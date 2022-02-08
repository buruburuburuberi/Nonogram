#pragma once

#include <nonogram/data/Answer.hpp>
#include <nonogram/data/Array2D.hpp>
#include <nonogram/data/Solution.hpp>

#include <QtCore/QString>

#include <algorithm>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

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
      Nonogram (QString pack, QString puzzle, Solution);
      Nonogram (QString pack, QString puzzle, Solution, Answer);

      QString pack() const;
      QString puzzle() const;
      Rows dataRows() const;
      Columns dataColumns() const;
      Solution::State solution (Slot) const;
      Answer::Datum answer (Slot) const;
      void fill (Slot, Answer::Datum);
      Slots dataToLock() const;
      Slots lockedData() const;
      bool isDatumLocked (Slot) const;
      void lockData (Slots, bool);

      MinorSize maxNumberOfClues (Clues::Type type) const;
      MainSize clueMainSize (Clues::Type) const;
      MinorSize clueMinorSize (Clues::Type, MainIndex) const;
      Clues::Value clue (Clues::Type, FullIndex) const;
      ClueState isCrossed (Clues::Type, FullIndex) const;
      void cross (Clues::Type, FullIndex, ClueState);
      Solution::ClueIndices cluesToLock() const;
      Solution::ClueIndices lockedClues() const;
      bool isClueLocked (Clues::Type, FullIndex) const;
      void lockClues (Solution::ClueIndices, bool);

      bool canLock() const;
      bool canUnlock() const;
      bool isEmpty() const;

      bool isMistake (Slot) const;
      std::optional<Slot> findFirstMistake() const;
      bool isSolved() const;
      void resetAnswer();

    private:
      QString pack_;
      QString puzzle_;
      Solution solution_;
      Answer answer_;

      friend class file::Puzzles;
    };
  }
}
