#include <nonogram/gui/command/Cross.hpp>

namespace nonogram
{
  namespace gui
  {
    namespace command
    {
      Cross::Cross ( int id
                   , data::Nonogram& nonogram
                   , data::Solution::ClueIndices clue_indices
                   , data::ClueState before
                   , data::ClueState after
                   )
      : Base (id, nonogram)
      , clue_indices_ (std::move (clue_indices))
      , before_ (before)
      , after_ (after)
      {
        setText ("Cross");
      }

      Cross* Cross::start ( data::Nonogram& nonogram
                          , data::Solution::ClueIndices clue_indices
                          , data::ClueState before
                          , data::ClueState after
                          )
      {
        return new Cross ( ++command_id
                         , nonogram
                         , std::move (clue_indices)
                         , before
                         , after
                         );
      }

      Cross* Cross::append ( data::Nonogram& nonogram
                           , data::Solution::ClueIndices clue_indices
                           , data::ClueState before
                           , data::ClueState after
                           )
      {
        return new Cross ( command_id
                         , nonogram
                         , std::move (clue_indices)
                         , before
                         , after
                         );
      }

      void Cross::undo()
      {
        for (auto const& indices_of_type : clue_indices_)
        {
          for (auto const& index : indices_of_type.second)
          {
            nonogram_.cross (indices_of_type.first, index, before_);
          }
        }
      }

      void Cross::redo()
      {
        for (auto const& indices_of_type : clue_indices_)
        {
          for (auto const& index : indices_of_type.second)
          {
            nonogram_.cross (indices_of_type.first, index, after_);
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

        for (auto const& indices_of_type : other_cross->clue_indices_)
        {
          clue_indices_.at (indices_of_type.first).insert
            (indices_of_type.second.begin(), indices_of_type.second.end());
        }

        return true;
      }
    }
  }
}
