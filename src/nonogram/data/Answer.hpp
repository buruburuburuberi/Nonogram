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

      Datum at (Slot) const;
      void set (Slot, Datum);

      bool is_crossed (Solution::ClueType, Slot) const;
      void set_crossed (Solution::ClueType, Slot, ClueState);

      void reset();

    private:
      Array2D<Datum> data_;
      std::map<Solution::ClueType, Array2D<ClueState>> clue_states_;
    };
  }
}
