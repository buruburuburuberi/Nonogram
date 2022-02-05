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

      using ClueIndices = std::map<Clues::Type, Indices>;

      Solution (Data);

      Rows dataRows() const;
      Columns dataColumns() const;
      State at (Slot) const;

      MinorSize maxNumberOfClues (Clues::Type) const;
      MainSize clueMainSize (Clues::Type) const;
      MinorSize clueMinorSize (Clues::Type, MainIndex) const;
      Clues::Value clue (Clues::Type, FullIndex) const;

    private:
      Data data_;
      std::map<Clues::Type, Clues> clues_;
    };
  }
}
