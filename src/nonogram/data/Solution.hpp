#pragma once

#include <nonogram/data/Array2D.hpp>

#include <QtCore/QSize>

#include <array>
#include <cstddef>
#include <map>

namespace nonogram
{
  namespace data
  {
    class Solution
    {
    public:
      using State = bool;
      using Clue = std::size_t;
      using Clues = Array2D<Clue>;

      enum class ClueType
      { Left
      , Top
      , Right
      , Bottom
      };

      static std::array<ClueType, 4> constexpr all_clue_types
      { ClueType::Left
      , ClueType::Top
      , ClueType::Right
      , ClueType::Bottom
      };

      using ClueSlots = std::map<ClueType, Slots>;

      Solution (Array2D<bool>);

      Rows dataRows() const;
      Columns dataColumns() const;
      State at (Slot) const;

      Columns clueColumns (ClueType) const;
      Rows clueRows (ClueType) const;

      Clue clue (ClueType, Slot) const;

    private:
      Clues computeRightClues() const;
      Clues computeBottomClues() const;
      Clues computeLeftClues (Clues const&) const;
      Clues computeTopClues (Clues const&) const;

      Array2D<bool> data_;
      std::map<ClueType, Clues> clues_;
    };
  }
}
