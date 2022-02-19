#include <nonogram/gui/command/Cross.hpp>

namespace nonogram::gui::command
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
    for (auto const& [type, full_indices] : clue_indices_)
    {
      for (auto const& index : full_indices)
      {
        nonogram_.cross (type, index, before_);
      }
    }
  }

  void Cross::redo()
  {
    for (auto const& [type, full_indices] : clue_indices_)
    {
      for (auto const& index : full_indices)
      {
        nonogram_.cross (type, index, after_);
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

    for (auto const& [type, indices] : other_cross->clue_indices_)
    {
      clue_indices_.at (type).insert (indices.begin(), indices.end());
    }

    return true;
  }
}
