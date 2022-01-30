#pragma once

#include <nonogram/data/Answer.hpp>
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
      Nonogram (std::string name, Solution);

      std::string name() const;
      Rows rows_of_data() const;
      Columns columns_of_data() const;
      Answer::Datum at (Column, Row) const;

      Columns columns_of_clues (Solution::ClueType) const;
      Rows rows_of_clues (Solution::ClueType) const;

      Solution::Clue clue (Solution::ClueType, Column, Row) const;
      Answer::ClueState is_clue_crossed (Solution::ClueType, Column, Row) const;

    private:
      std::string name_;
      Solution solution_;
      Answer answer_;
    };
  }
}
