#include <nonogram/data/Nonogram.hpp>

#include <cstddef>
#include <string>
#include <vector>

namespace nonogram
{
  namespace data
  {
    Nonogram::Nonogram (std::string name, Solution data)
    : name_ (std::move (name))
    , solution_ (std::move (data))
    , answer_ (solution_)
    {}

    std::string Nonogram::name() const
    {
      return name_;
    }

    Rows Nonogram::rows_of_data() const
    {
      return solution_.rows_of_data();
    }

    Columns Nonogram::columns_of_data() const
    {
      return solution_.columns_of_data();
    }

    Answer::Datum Nonogram::at (Slot slot) const
    {
      return answer_.at (slot);
    }

    void Nonogram::set (Slot slot, Answer::Datum datum)
    {
      answer_.set (slot, datum);
    }

    Columns Nonogram::columns_of_clues (Solution::ClueType type) const
    {
      return solution_.columns_of_clues (type);
    }

    Rows Nonogram::rows_of_clues (Solution::ClueType type) const
    {
      return solution_.rows_of_clues (type);
    }

    Solution::Clue Nonogram::clue ( Solution::ClueType type
                                  , Slot slot
                                  ) const
    {
      return solution_.clue (type, slot);
    }

    Answer::ClueState Nonogram::is_crossed ( Solution::ClueType type
                                           , Slot slot
                                           ) const
    {
      return answer_.is_crossed (type, slot);
    }

    void Nonogram::set_crossed ( Solution::ClueType type
                               , Slot slot
                               , Answer::ClueState state
                               )
    {
      answer_.set_crossed (type, slot, state);
    }

    bool Nonogram::isMistake (Slot slot) const
    {
      auto const datum (answer_.at (slot));
      return ((datum == Answer::Datum::Filled) || (datum == Answer::Datum::Crossed))
          && solution_.at (slot) != (datum == data::Answer::Datum::Filled);
    }

    std::optional<Slot> Nonogram::findFirstMistake() const
    {
      for ( Column column {0}
          ; column.value < columns_of_data().value
          ; ++column.value
          )
      {
        for (Row row {0}; row.value < rows_of_data().value; ++row.value)
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
          ; column.value < columns_of_data().value
          ; ++column.value
          )
      {
        for (Row row {0}; row.value < rows_of_data().value; ++row.value)
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
