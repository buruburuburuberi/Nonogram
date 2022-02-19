#pragma once

#include <nonogram/data/Nonogram.hpp>

#include <QtWidgets/QUndoCommand>

namespace nonogram::gui::command
{
  class Base : public QUndoCommand
  {
  public:
    Base (int id, data::Nonogram&);

    int id() const override;

  protected:
    int const id_;
    data::Nonogram& nonogram_;
    static inline int command_id {0};
  };
}
