#pragma once

#include <nonogram/gui/command/Base.hpp>
#include <nonogram/gui/command/Cross.hpp>
#include <nonogram/gui/command/Fill.hpp>
#include <nonogram/data/Array2D.hpp>
#include <nonogram/data/Answer.hpp>

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
        data::Slots data_slots_;
        data::Solution::ClueIndices clue_indices_;
      };
    }
  }
}
