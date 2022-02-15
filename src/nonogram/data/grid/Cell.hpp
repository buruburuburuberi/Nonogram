#pragma once

#include <nonogram/data/grid/Column.hpp>
#include <nonogram/data/grid/Row.hpp>

namespace nonogram
{
  namespace data
  {
    namespace grid
    {
      struct Cell
      {
        Column column;
        Row row;

        bool operator== (Cell const& rhs) const;
        bool operator< (Cell const& rhs) const;
      };
    }
  }
}
