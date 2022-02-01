#pragma once

#include <nonogram/gui/command/Base.hpp>
#include <nonogram/data/Array2D.hpp>
#include <nonogram/data/Answer.hpp>

#include <QtWidgets/QUndoCommand>

#include <vector>

namespace nonogram
{
  namespace gui
  {
    namespace command
    {
      class Fill : public Base
      {
      public:
        static Fill* start ( data::Nonogram&
                           , data::Slots
                           , data::Answer::Datum before
                           , data::Answer::Datum after
                           );

        static Fill* append ( data::Nonogram&
                            , data::Slots
                            , data::Answer::Datum before
                            , data::Answer::Datum after
                            );

        void undo() override;
        void redo() override;

        bool mergeWith (QUndoCommand const*);

      private:
        Fill ( int id
             , data::Nonogram&
             , data::Slots
             , data::Answer::Datum before
             , data::Answer::Datum after
             );

        data::Slots data_slots_;
        data::Answer::Datum before_;
        data::Answer::Datum after_;
      };
    }
  }
}
