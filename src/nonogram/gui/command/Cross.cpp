#include <nonogram/gui/command/Cross.hpp>

namespace nonogram
{
  namespace gui
  {
    namespace command
    {
      Cross::Cross ( int id
                   , data::Nonogram& nonogram
                   , data::Solution::ClueSlots clue_slots
                   , data::Answer::ClueState before
                   , data::Answer::ClueState after
                   )
      : Base (id, nonogram)
      , clue_slots_ (std::move (clue_slots))
      , before_ (before)
      , after_ (after)
      {
        setText ("Cross");
      }

      Cross* Cross::start ( data::Nonogram& nonogram
                          , data::Solution::ClueSlots clue_slots
                          , data::Answer::ClueState before
                          , data::Answer::ClueState after
                          )
      {
        return new Cross ( ++command_id
                         , nonogram
                         , std::move (clue_slots)
                         , before
                         , after
                         );
      }

      Cross* Cross::append ( data::Nonogram& nonogram
                           , data::Solution::ClueSlots clue_slots
                           , data::Answer::ClueState before
                           , data::Answer::ClueState after
                           )
      {
        return new Cross ( command_id
                         , nonogram
                         , std::move (clue_slots)
                         , before
                         , after
                         );
      }

      void Cross::undo()
      {
        for (auto const& clue_slot : clue_slots_)
        {
          for (auto const& slot : clue_slot.second)
          {
            nonogram_.cross (clue_slot.first, slot, before_);
          }
        }
      }

      void Cross::redo()
      {
        for (auto const& clue_slot : clue_slots_)
        {
          for (auto const& slot : clue_slot.second)
          {
            nonogram_.cross (clue_slot.first, slot, after_);
          }
        }
      }

      bool Cross::mergeWith (QUndoCommand const* other)
      {
        if (other->id() != id())
        {
          return false;
        }

        auto const other_cross (static_cast<Cross const*> (other));
        if (other_cross->before_ != before_ || other_cross->after_ != after_)
        {
          return false;
        }

        for (auto const& clue_slot : other_cross->clue_slots_)
        {
          clue_slots_.at (clue_slot.first).insert
            (clue_slot.second.begin(), clue_slot.second.end());
        }

        return true;
      }
    }
  }
}
