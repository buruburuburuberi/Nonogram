#pragma once

#include <nonogram/gui/command/Base.hpp>
#include <nonogram/data/Array2D.hpp>
#include <nonogram/data/Answer.hpp>
#include <nonogram/data/Solution.hpp>

#include <QtWidgets/QUndoCommand>

#include <map>
#include <vector>

namespace nonogram
{
  namespace gui
  {
    namespace command
    {
      class Cross : public Base
      {
      public:
        static Cross* start ( data::Nonogram&
                            , data::Solution::ClueSlots
                            , data::Answer::ClueState before
                            , data::Answer::ClueState after
                            );

        static Cross* append ( data::Nonogram&
                             , data::Solution::ClueSlots
                             , data::Answer::ClueState before
                             , data::Answer::ClueState after
                             );

        void undo() override;
        void redo() override;

        bool mergeWith (QUndoCommand const*);

      private:
        Cross ( int id
              , data::Nonogram&
              , data::Solution::ClueSlots
              , data::Answer::ClueState before
              , data::Answer::ClueState after
              );

        data::Solution::ClueSlots clue_slots_;
        data::Answer::ClueState before_;
        data::Answer::ClueState after_;
      };
    }
  }
}
