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

    unsigned int Nonogram::rows() const
    {
      return solution_.rows();
    }

    unsigned int Nonogram::columns() const
    {
      return solution_.columns();
    }

    Nonogram::Datum Nonogram::at (unsigned int column, unsigned int row) const
    {
      return answer_.at (column, row);
    }
  }
}
