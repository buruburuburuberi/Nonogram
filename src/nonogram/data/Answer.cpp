#include <nonogram/data/Answer.hpp>

#include <string>

namespace nonogram
{
  namespace data
  {
    Answer::Answer (Solution const& solution)
    : data_ (solution.dataColumns(), solution.dataRows())
    {
      reset();

      for (auto const& type : Solution::all_clue_types)
      {
        clue_states_.emplace
          ( std::piecewise_construct
          , std::make_tuple (type)
          , std::make_tuple
              ( solution.clueColumns (type)
              , solution.clueRows (type)
              , false
              )
          );
      }
    }

    Answer::Datum Answer::at (Slot slot) const
    {
      return data_.at (slot);
    }

    void Answer::fill (Slot slot, Datum datum)
    {
      data_.set (slot, datum);
    }

    Answer::ClueState Answer::isCrossed ( Solution::ClueType type
                                        , Slot slot
                                        ) const
    {
      return clue_states_.at (type).at (slot);
    }

    void Answer::cross ( Solution::ClueType type
                       , Slot slot
                       , ClueState state
                       )
    {
      clue_states_.at (type).set (slot, state);
    }

    void Answer::reset()
    {
      data_.fill (Answer::Datum::Empty);
      for (auto& clue_states : clue_states_)
      {
        clue_states.second.fill (false);
      }
    }
  }
}
