#include <nonogram/data/Nonogram.hpp>

#include <cstddef>
#include <string>
#include <vector>

namespace nonogram
{
  namespace data
  {
    Nonogram::Nonogram (std::string name, Solution solution)
    : name_ (std::move (name))
    , solution_ (std::move (solution))
    , answer_ (solution_)
    {}

    std::string Nonogram::name() const
    {
      return name_;
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

    void Nonogram::fill (Slot slot, Answer::Datum datum)
    {
      answer_.fill (slot, datum);
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

    bool Nonogram::canLock() const
    {
      return answer_.canLock();
    }

    bool Nonogram::canUnlock() const
    {
      return answer_.canUnlock();
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
