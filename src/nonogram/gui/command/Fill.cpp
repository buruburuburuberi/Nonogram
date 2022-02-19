#include <nonogram/gui/command/Fill.hpp>

namespace nonogram::gui::command
{
  Fill::Fill ( int id
             , data::Nonogram& nonogram
             , data::grid::Cells cells
             , data::Answer::Datum before
             , data::Answer::Datum after
             )
  : Base (id, nonogram)
  , cells_ (std::move (cells))
  , before_ (before)
  , after_ (after)
  {
    setText ("Fill");
  }

  Fill* Fill::start ( data::Nonogram& nonogram
                    , data::grid::Cells cells
                    , data::Answer::Datum before
                    , data::Answer::Datum after
                    )
  {
    return new Fill ( ++command_id
                    , nonogram
                    , std::move (cells)
                    , before
                    , after
                    );
  }

  Fill* Fill::append ( data::Nonogram& nonogram
                     , data::grid::Cells cells
                     , data::Answer::Datum before
                     , data::Answer::Datum after
                     )
  {
    return new Fill ( command_id
                    , nonogram
                    , std::move (cells)
                    , before
                    , after
                    );
  }

  void Fill::undo()
  {
    for (auto const& cell : cells_)
    {
      nonogram_.fillData (cell, before_);
    }
  }

  void Fill::redo()
  {
    for (auto const& cell : cells_)
    {
      nonogram_.fillData (cell, after_);
    }
  }

  bool Fill::mergeWith (QUndoCommand const* other)
  {
    if (other->id() != id())
    {
      return false;
    }

    auto const other_fill (static_cast<Fill const*> (other));
    if (other_fill->before_ != before_ || other_fill->after_ != after_)
    {
      return false;
    }

    cells_.insert (other_fill->cells_.begin(), other_fill->cells_.end());

    return true;
  }
}
