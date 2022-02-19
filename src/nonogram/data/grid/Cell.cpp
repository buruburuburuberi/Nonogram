#include <nonogram/data/grid/Cell.hpp>

#include <tuple>

namespace nonogram::data::grid
{
  bool Cell::operator== (Cell const& rhs) const
  {
    return std::tie (row, column) == std::tie (rhs.row, rhs.column);
  }

  bool Cell::operator< (Cell const& rhs) const
  {
    return std::tie (row, column) < std::tie (rhs.row, rhs.column);
  }
}
