#include <nonogram/gui/command/Fill.hpp>

namespace nonogram
{
  namespace gui
  {
    namespace command
    {
      Fill::Fill ( int id
                 , data::Nonogram& nonogram
                 , data::Slots data_slots
                 , data::Answer::Datum before
                 , data::Answer::Datum after
                 )
      : Base (id, nonogram)
      , data_slots_ (std::move (data_slots))
      , before_ (before)
      , after_ (after)
      {
        setText ("Fill");
      }

      Fill* Fill::start ( data::Nonogram& nonogram
                        , data::Slots data_slots
                        , data::Answer::Datum before
                        , data::Answer::Datum after
                        )
      {
        return new Fill ( ++command_id
                        , nonogram
                        , std::move (data_slots)
                        , before
                        , after
                        );
      }

      Fill* Fill::append ( data::Nonogram& nonogram
                         , data::Slots data_slots
                         , data::Answer::Datum before
                         , data::Answer::Datum after
                         )
      {
        return new Fill ( command_id
                        , nonogram
                        , std::move (data_slots)
                        , before
                        , after
                        );
      }

      void Fill::undo()
      {
        for (auto const& slot : data_slots_)
        {
          nonogram_.fill (slot, before_);
        }
      }

      void Fill::redo()
      {
        for (auto const& slot : data_slots_)
        {
          nonogram_.fill (slot, after_);
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

        data_slots_.insert ( other_fill->data_slots_.begin()
                           , other_fill->data_slots_.end()
                           );

        return true;
      }
    }
  }
}
