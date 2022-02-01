#include <nonogram/data/Answer.hpp>

#include <string>

namespace nonogram
{
  namespace data
  {
    Answer::Answer (Solution const& solution)
    : data_ (solution.columns_of_data(), solution.rows_of_data())
    {
      reset();

      clue_states_.emplace
        ( std::piecewise_construct
        , std::make_tuple (Solution::ClueType::Left)
        , std::make_tuple
            ( solution.columns_of_clues (Solution::ClueType::Left)
            , solution.rows_of_clues (Solution::ClueType::Left)
            , false
            )
        );
      clue_states_.emplace
        ( std::piecewise_construct
        , std::make_tuple (Solution::ClueType::Top)
        , std::make_tuple
            ( solution.columns_of_clues (Solution::ClueType::Top)
            , solution.rows_of_clues (Solution::ClueType::Top)
            , false
            )
        );
      clue_states_.emplace
        ( std::piecewise_construct
        , std::make_tuple (Solution::ClueType::Right)
        , std::make_tuple
            ( solution.columns_of_clues (Solution::ClueType::Right)
            , solution.rows_of_clues (Solution::ClueType::Right)
            , false
            )
        );
      clue_states_.emplace
        ( std::piecewise_construct
        , std::make_tuple (Solution::ClueType::Bottom)
        , std::make_tuple
            ( solution.columns_of_clues (Solution::ClueType::Bottom)
            , solution.rows_of_clues (Solution::ClueType::Bottom)
            , false
            )
        );
    }

    Answer::Datum Answer::at (Slot slot) const
    {
      return data_.at (slot);
    }

    void Answer::set (Slot slot, Datum datum)
    {
      data_.set (slot, datum);
    }

    Answer::ClueState Answer::is_crossed ( Solution::ClueType type
                                         , Slot slot
                                         ) const
    {
      return clue_states_.at (type).at (slot);
    }

    void Answer::set_crossed ( Solution::ClueType type
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
