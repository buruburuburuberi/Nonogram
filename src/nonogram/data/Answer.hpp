#pragma once

#include <nonogram/data/Array2D.hpp>
#include <nonogram/data/Solution.hpp>

#include <QtCore/QSize>

#include <cstddef>
#include <map>

namespace nonogram
{
  namespace data
  {
    class Answer
    {
    public:
      enum class Datum
      { Empty = 0
      , Filled = 1
      , Crossed = 2
      , FillMark = 3
      , CrossMark = 4
      };

      using ClueState = bool;

      Answer (Solution const&);

      Datum at (Column, Row) const;

      bool is_clue_crossed (Solution::ClueType, Column, Row) const;
      void set_clue_crossed (Solution::ClueType, Column, Row, ClueState);

    private:
      Array2D<Datum> data_;
      std::map<Solution::ClueType, Array2D<ClueState>> clue_states_;
    };
  }
}
