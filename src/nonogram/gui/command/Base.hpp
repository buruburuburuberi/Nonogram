#pragma once

#include <nonogram/data/Nonogram.hpp>

#include <QtWidgets/QUndoCommand>

namespace nonogram
{
  namespace gui
  {
    namespace command
    {
      static int command_id = 0;

      class Base : public QUndoCommand
      {
      public:
        Base (int id, data::Nonogram&);

        int id() const override;

      protected:
        int const id_;
        data::Nonogram& nonogram_;
      };
    }
  }
}
