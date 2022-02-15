#include <nonogram/data/grid/Cell.hpp>

#include <tuple>

namespace nonogram
{
  namespace data
  {
    namespace grid
    {
      bool Cell::operator== (Cell const& rhs) const
      {
        return std::tie (column, row) == std::tie (rhs.column, rhs.row);
      }

      bool Cell::operator< (Cell const& rhs) const
      {
        return std::tie (row, column) < std::tie (rhs.row, rhs.column);
      }
    }
  }
}
