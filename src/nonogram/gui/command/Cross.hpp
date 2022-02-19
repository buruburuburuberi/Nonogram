#pragma once

#include <nonogram/gui/command/Base.hpp>
#include <nonogram/data/ClueStates.hpp>
#include <nonogram/data/Nonogram.hpp>
#include <nonogram/data/Solution.hpp>

#include <QtWidgets/QUndoCommand>

namespace nonogram::gui::command
{
  class Cross : public Base
  {
  public:
    static Cross* start ( data::Nonogram&
                        , data::Solution::ClueIndices
                        , data::ClueState before
                        , data::ClueState after
                        );

    static Cross* append ( data::Nonogram&
                         , data::Solution::ClueIndices
                         , data::ClueState before
                         , data::ClueState after
                         );

    void undo() override;
    void redo() override;

    bool mergeWith (QUndoCommand const*);

  private:
    Cross ( int id
          , data::Nonogram&
          , data::Solution::ClueIndices
          , data::ClueState before
          , data::ClueState after
          );

    data::Solution::ClueIndices clue_indices_;
    data::ClueState before_;
    data::ClueState after_;
  };
}
