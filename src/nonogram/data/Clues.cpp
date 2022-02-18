#include <nonogram/data/Clues.hpp>

#include <algorithm>

namespace nonogram
{
  namespace data
  {
    clues::FullIndex Clues::toFullIndex (Type type, grid::Cell cell)
    {
      return { clues::MainIndex
                 {type == Type::Column ?  cell.column.value : cell.row.value}
             , clues::MinorIndex
                 {type == Type::Column ? cell.row.value : cell.column.value}
             };
    }

    Clues::Clues (grid::Data<bool> const& data, Type type)
    : type_ (type)
    , max_minor_count_ (0)
    , data_ (computeClues (data, type))
    {}

    Clues::Data Clues::computeClues (grid::Data<bool> const& data, Type type)
    {
      auto const clues_size {toFullIndex (type, {data.columns(), data.rows()})};

      std::vector<std::vector<Value>> clues (clues_size.main.value);

      auto is_filled
        ( [&] (clues::MainIndex main, clues::MinorIndex minor)
          {
            return type == Type::Column
              ? data.at (grid::Column {main.value}, grid::Row {minor.value})
              : data.at (grid::Column {minor.value}, grid::Row {main.value})
              ;
          }
        );

      for (clues::MainIndex main {0}; main < clues_size.main; ++main)
      {
        std::size_t filled_counter (0);
        for (clues::MinorIndex minor {0}; minor < clues_size.minor; ++minor)
        {
          auto const current_square_filled (is_filled (main, minor));

          if (current_square_filled)
          {
            ++filled_counter;
          }

          if ( minor.value > 0
            && !current_square_filled
            && is_filled (main, minor - clues::MinorIndex (1))
             )
          {
            clues.at (main.value).push_back (filled_counter);
            filled_counter = 0;
          }

          if ( (minor == clues_size.minor - clues::MinorIndex (1))
            && (filled_counter > 0)
             )
          {
            clues.at (main.value).push_back (filled_counter);
          }
        }

        max_minor_count_ =
            std::max ( max_minor_count_
                     , clues::MinorIndex (clues.at (main.value).size())
                     );
      }

      return clues;
    }

    clues::MinorIndex Clues::maxNumberOfClues() const
    {
      return max_minor_count_;
    }

    clues::MainIndex Clues::mainSize() const
    {
      return data_.mainSize();
    }

    clues::MinorIndex Clues::minorSize (clues::MainIndex main_index) const
    {
      return data_.minorSize (main_index);
    }

    Clues::Value Clues::clue (clues::FullIndex full_index) const
    {
      return data_.at (full_index);
    }
  }
}
