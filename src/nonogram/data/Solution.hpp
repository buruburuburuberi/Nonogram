#pragma once

#include <nonogram/data/Array2D.hpp>

#include <QtCore/QSize>

#include <cstddef>
#include <map>

namespace nonogram
{
  namespace data
  {
    class Solution
    {
    public:
      using Clue = std::size_t;
      using Clues = Array2D<Clue>;

      enum class ClueType
      { Left = 0
      , Top = 1
      , Right = 2
      , Bottom = 3
      };

      Solution (Array2D<bool>);

      Rows rows_of_data() const;
      Columns columns_of_data() const;

      Columns columns_of_clues (ClueType) const;
      Rows rows_of_clues (ClueType) const;

      Clue clue (ClueType, Column, Row) const;

    private:
      Clues compute_right_clues() const;
      Clues compute_bottom_clues() const;
      Clues compute_left_clues (Clues const&) const;
      Clues compute_top_clues (Clues const&) const;

      Array2D<bool> data_;
      std::map<ClueType, Clues> clues_;
    };
  }
}
