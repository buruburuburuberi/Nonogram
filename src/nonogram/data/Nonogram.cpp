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
    , answer_ (solution_.columns(), solution_.rows())
    {
      answer_.fill (Datum::Empty);
    }

    std::string Nonogram::name() const
    {
      return name_;
    }

    Rows Nonogram::rows() const
    {
      return solution_.rows();
    }

    Columns Nonogram::columns() const
    {
      return solution_.columns();
    }

    Nonogram::Datum Nonogram::at (Column column, Row row) const
    {
      return answer_.at (column, row);
    }

    QSize Nonogram::size_of_clues() const
    {
      return solution_.size_of_clues();
    }

    Solution::Clues const& Nonogram::clues (Column column) const
    {
      return solution_.clues (column);
    }

    Solution::Clues const& Nonogram::clues (Row row) const
    {
      return solution_.clues (row);
    }
  }
}
