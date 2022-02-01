#pragma once

#include <nonogram/data/Answer.hpp>
#include <nonogram/data/Array2D.hpp>
#include <nonogram/data/Solution.hpp>

#include <algorithm>
#include <cstddef>
#include <optional>
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
      Rows dataRows() const;
      Columns dataColumns() const;
      Answer::Datum at (Slot) const;
      void fill (Slot, Answer::Datum);

      Columns clueColumns (Solution::ClueType) const;
      Rows clueRows (Solution::ClueType) const;

      Solution::Clue clue (Solution::ClueType, Slot) const;
      Answer::ClueState isCrossed (Solution::ClueType, Slot) const;
      void cross (Solution::ClueType, Slot, Answer::ClueState);

      bool isMistake (Slot) const;
      std::optional<Slot> findFirstMistake() const;
      bool isSolved() const;
      void resetAnswer();

    private:
      std::string name_;
      Solution solution_;
      Answer answer_;
    };
  }
}
