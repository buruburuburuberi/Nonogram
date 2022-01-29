#pragma once

#include <nonogram/data/Array2D.hpp>
#include <nonogram/data/Solution.hpp>

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

namespace nonogram
{
  namespace data
  {
    class Nonogram
    {
    public:
      enum class Datum
      { Empty = 0
      , Filled = 1
      , Crossed = 2
      , FillMark = 3
      , CrossMark = 4
      };
;
      using Answer = Array2D<Datum>;

      Nonogram (std::string name, Solution);

      std::string name() const;
      Rows rows() const;
      Columns columns() const;
      Datum at (Column, Row) const;

      QSize size_of_clues() const;
      Solution::Clues const& clues (Column) const;
      Solution::Clues const& clues (Row) const;

    private:
      std::string name_;
      Solution solution_;
      Answer answer_;
    };
  }
}
