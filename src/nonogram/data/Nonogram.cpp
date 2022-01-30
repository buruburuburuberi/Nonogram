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

    Answer::Datum Nonogram::at (Column column, Row row) const
    {
      return answer_.at (column, row);
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
                                  , Column column
                                  , Row row
                                  ) const
    {
      return solution_.clue (type, column, row);
    }

    Answer::ClueState Nonogram::is_clue_crossed ( Solution::ClueType type
                                                , Column column
                                                , Row row
                                                ) const
    {
      return answer_.is_clue_crossed (type, column, row);
    }
  }
}
