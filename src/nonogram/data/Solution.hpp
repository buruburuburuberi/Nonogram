#pragma once

#include <nonogram/data/clues/Data.hpp>
#include <nonogram/data/grid/Data.hpp>
#include <nonogram/data/Clues.hpp>

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
      using Data = grid::Data<bool>;

      using ClueIndices = std::map<Clues::Type, clues::FullIndices>;

      Solution (Data);

      grid::Row dataRows() const;
      grid::Column dataColumns() const;
      State at (grid::Cell) const;

      clues::MinorIndex maxNumberOfClues (Clues::Type) const;
      clues::MainIndex clueMainSize (Clues::Type) const;
      clues::MinorIndex clueMinorSize (Clues::Type, clues::MainIndex) const;
      Clues::Value clue (Clues::Type, clues::FullIndex) const;

    private:
      Data data_;
      std::map<Clues::Type, Clues> clues_;
    };
  }
}
