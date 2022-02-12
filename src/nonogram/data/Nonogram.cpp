#include <nonogram/data/Nonogram.hpp>

#include <QtCore/QStringList>

#include <cstddef>
#include <string>
#include <tuple>
#include <vector>

namespace nonogram
{
  namespace data
  {
    bool Nonogram::Pack::operator== (Pack const& rhs) const
    {
      return name == rhs.name;
    }
    bool Nonogram::Pack::operator< (Pack const& rhs) const
    {
      return name < rhs.name;
    }

    bool Nonogram::Puzzle::operator== (Puzzle const& rhs) const
    {
      return name == rhs.name;
    }
    bool Nonogram::Puzzle::operator< (Puzzle const& rhs) const
    {
      return name < rhs.name;
    }

    Nonogram::ID::ID (Pack _pack, Puzzle _puzzle)
    : pack {_pack.name}
    , puzzle {_puzzle.name}
    {}

    Nonogram::ID::ID (QString string)
    {
      auto const strings (string.split ("/"));
      pack.name = strings.at (0);
      puzzle.name = strings.at (1);
    }

    QString Nonogram::ID::toString() const
    {
      return QString ("%1/%2").arg (pack.name).arg (puzzle.name);
    }

    bool Nonogram::ID::operator== (ID const& rhs) const
    {
      return std::tie (pack, puzzle) == std::tie (rhs.pack, rhs.puzzle);
    }
    bool Nonogram::ID::operator< (ID const& rhs) const
    {
      if (pack < rhs.pack)
      {
        return true;
      }
      else if (pack == rhs.pack)
      {
        return puzzle < rhs.puzzle;
      }
      else
      {
        return false;
      }
    }

    Nonogram::Nonogram ( data::Nonogram::ID id
                       , Solution solution
                       , Answer answer
                       )
    : id_ (std::move (id))
    , solution_ (std::move (solution))
    , answer_ (std::move (answer))
    {}

    Nonogram::Nonogram (data::Nonogram::ID id, Solution solution)
    : Nonogram (std::move (id), solution, Answer (solution))
    {}

    Nonogram::ID Nonogram::id() const
    {
      return id_;
    }

    Rows Nonogram::dataRows() const
    {
      return solution_.dataRows();
    }

    Columns Nonogram::dataColumns() const
    {
      return solution_.dataColumns();
    }

    Solution::State Nonogram::solution (Slot slot) const
    {
      return solution_.at (slot);
    }

    Answer::Datum Nonogram::answer (Slot slot) const
    {
      return answer_.at (slot);
    }

    void Nonogram::fillData (Slot slot, Answer::Datum datum)
    {
      answer_.fillData (slot, datum);
    }

    Slots Nonogram::dataToLock() const
    {
      return answer_.dataToLock();
    }

    Slots Nonogram::lockedData() const
    {
      return answer_.lockedData();
    }

    bool Nonogram::isDatumLocked (Slot slot) const
    {
      return answer_.isDatumLocked (slot);
    }

    void Nonogram::lockData (Slots slots, bool state)
    {
      answer_.lockData (std::move (slots), state);
    }

    void Nonogram::fillDataLocks (bool state)
    {
      answer_.fillDataLocks (state);
    }

    MinorSize Nonogram::maxNumberOfClues (Clues::Type type) const
    {
      return solution_.maxNumberOfClues (type);
    }

    MainSize Nonogram::clueMainSize (Clues::Type type) const
    {
      return solution_.clueMainSize (type);
    }

    MinorSize Nonogram::clueMinorSize (Clues::Type type, MainIndex main_index) const
    {
      return solution_.clueMinorSize (type, main_index);
    }

    Clues::Value Nonogram::clue (Clues::Type type, FullIndex full_index) const
    {
      return solution_.clue (type, full_index);
    }

    ClueState Nonogram::isCrossed (Clues::Type type, FullIndex full_index) const
    {
      return answer_.isCrossed (type, full_index);
    }

    void Nonogram::cross (Clues::Type type, FullIndex full_index, ClueState state)
    {
      answer_.cross (type, full_index, state);
    }

    Solution::ClueIndices Nonogram::cluesToLock() const
    {
      return answer_.cluesToLock();
    }

    Solution::ClueIndices Nonogram::lockedClues() const
    {
      return answer_.lockedClues();
    }

    bool Nonogram::isClueLocked (Clues::Type type, FullIndex full_index) const
    {
      return answer_.isClueLocked (type, full_index);
    }

    void Nonogram::lockClues (Solution::ClueIndices indices, bool state)
    {
      answer_.lockClues (std::move (indices), state);
    }

    void Nonogram::fillClueLocks (bool state)
    {
      answer_.fillClueLocks (state);
    }

    bool Nonogram::canLock() const
    {
      return answer_.canLock();
    }

    bool Nonogram::canUnlock() const
    {
      return answer_.canUnlock();
    }

    bool Nonogram::isEmpty() const
    {
      return answer_.isEmpty();
    }

    bool Nonogram::isMistake (Slot slot) const
    {
      auto const datum (answer_.at (slot));
      return ( (datum == Answer::Datum::Filled)
            || (datum == Answer::Datum::Crossed)
             )
          && solution_.at (slot) != (datum == data::Answer::Datum::Filled);
    }

    std::optional<Slot> Nonogram::findFirstMistake() const
    {
      for (Row row {0}; row.value < dataRows().value; ++row.value)
      {
        for ( Column column {0}
            ; column.value < dataColumns().value
            ; ++column.value
            )
        {
          Slot const slot {column, row};
          if (isMistake (slot))
          {
            return slot;
          }
        }
      }

      return std::nullopt;
    }

    bool Nonogram::isSolved() const
    {
      for (Row row {0}; row.value < dataRows().value; ++row.value)
      {
        for ( Column column {0}
            ; column.value < dataColumns().value
            ; ++column.value
            )
        {
          Slot const slot {column, row};
          auto const datum (answer_.at (slot));
          if (solution_.at (slot) != (datum == data::Answer::Datum::Filled))
          {
            return false;
          }
        }
      }

      return true;
    }

    void Nonogram::resetAnswer()
    {
      answer_.reset();
    }
  }
}
