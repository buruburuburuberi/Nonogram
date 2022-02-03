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

    Answer::Datum Nonogram::at (Slot slot) const
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

    Columns Nonogram::clueColumns (Solution::ClueType type) const
    {
      return solution_.clueColumns (type);
    }

    Rows Nonogram::clueRows (Solution::ClueType type) const
    {
      return solution_.clueRows (type);
    }

    Solution::Clue Nonogram::clue ( Solution::ClueType type
                                  , Slot slot
                                  ) const
    {
      return solution_.clue (type, slot);
    }

    Answer::ClueState Nonogram::isCrossed ( Solution::ClueType type
                                          , Slot slot
                                          ) const
    {
      return answer_.isCrossed (type, slot);
    }

    void Nonogram::cross ( Solution::ClueType type
                         , Slot slot
                         , Answer::ClueState state
                         )
    {
      answer_.cross (type, slot, state);
    }

    Solution::ClueSlots Nonogram::cluesToLock() const
    {
      return answer_.cluesToLock();
    }

    Solution::ClueSlots Nonogram::lockedClues() const
    {
      return answer_.lockedClues();
    }

    bool Nonogram::isClueLocked (Solution::ClueType type, Slot slot) const
    {
      return answer_.isClueLocked (type, slot);
    }

    void Nonogram::lockClues (Solution::ClueSlots slots, bool state)
    {
      answer_.lockClues (std::move (slots), state);
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
