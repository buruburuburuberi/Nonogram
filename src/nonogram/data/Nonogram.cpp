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
      for ( Column column {0}
          ; column.value < dataColumns().value
          ; ++column.value
          )
      {
        for (Row row {0}; row.value < dataRows().value; ++row.value)
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
      for ( Column column {0}
          ; column.value < dataColumns().value
          ; ++column.value
          )
      {
        for (Row row {0}; row.value < dataRows().value; ++row.value)
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
