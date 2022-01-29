#pragma once

#include <nonogram/data/Array2D.hpp>

#include <QtCore/QSize>

#include <cstddef>
#include <vector>

namespace nonogram
{
  namespace data
  {
    class Solution
    {
    public:
      using Clue = std::size_t;
      using Clues = std::vector<Clue>;

      Solution (Array2D<bool>);

      Rows rows() const;
      Columns columns() const;

      QSize size_of_clues() const;
      Clues const& clues (Column) const;
      Clues const& clues (Row) const;

    private:
      Array2D<bool> data_;
      std::vector<Clues> clues_of_columns_;
      std::vector<Clues> clues_of_rows_;
      QSize size_of_clues_;
    };
  }
}
