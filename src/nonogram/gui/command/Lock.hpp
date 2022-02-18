#pragma once

#include <nonogram/gui/command/Base.hpp>
#include <nonogram/data/Nonogram.hpp>
#include <nonogram/data/Solution.hpp>
#include <nonogram/data/grid/Cells.hpp>

#include <map>
#include <optional>
#include <vector>

namespace nonogram
{
  namespace gui
  {
    namespace command
    {
      class Lock : public Base
      {
      public:
        static Lock* lock (data::Nonogram&);
        static Lock* unlock (data::Nonogram&);

        void undo() override;
        void redo() override;

      private:
        Lock (data::Nonogram&, bool unlock);

        bool const unlock_;
        data::grid::Cells data_cells_;
        data::Solution::ClueIndices clue_indices_;
      };
    }
  }
}
