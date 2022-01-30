#include <nonogram/data/Answer.hpp>

#include <string>

namespace nonogram
{
  namespace data
  {
    Answer::Answer (Solution const& solution)
    : data_ (solution.columns_of_data(), solution.rows_of_data())
    {
      data_.fill (Answer::Datum::Empty);

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

    Answer::Datum Answer::at (Column column, Row row) const
    {
      return data_.at (column, row);
    }

    void Answer::set (Column column, Row row, Datum datum)
    {
      data_.set (column, row, datum);
    }

    Answer::ClueState Answer::is_crossed ( Solution::ClueType type
                                         , Column column
                                         , Row row
                                         ) const
    {
      return clue_states_.at (type).at (column, row);
    }

    void Answer::set_crossed ( Solution::ClueType type
                             , Column column
                             , Row row
                             , ClueState state
                             )
    {
      clue_states_.at (type).set (column, row, state);
    }
  }
}
