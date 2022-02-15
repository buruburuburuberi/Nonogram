#pragma once

#include <nonogram/data/Array2D.hpp>
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
      using Data = Array2D<bool>;

      using ClueIndices = std::map<Clues::Type, FullIndices>;

      Solution (Data);

      Row dataRows() const;
      Column dataColumns() const;
      State at (Slot) const;

      MinorIndex maxNumberOfClues (Clues::Type) const;
      MainIndex clueMainSize (Clues::Type) const;
      MinorIndex clueMinorSize (Clues::Type, MainIndex) const;
      Clues::Value clue (Clues::Type, FullIndex) const;

    private:
      Data data_;
      std::map<Clues::Type, Clues> clues_;
    };
  }
}
