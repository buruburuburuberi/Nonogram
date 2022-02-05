#include <nonogram/data/Clues.hpp>

#include <iostream>
#include <stdexcept>
#include <string>

namespace nonogram
{
  namespace data
  {
    FullIndex Clues::toFullIndex (Type type, Slot slot)
    {
      return
        { MainIndex {type == Type::Column ?  slot.column.value : slot.row.value}
        , MinorIndex {type == Type::Column ? slot.row.value : slot.column.value}
        };
    }

    Clues::Clues (Array2D<bool> const& data, Type type)
    : type_ (type)
    , max_minor_size_ {0}
    , data_ (computeClues (data, type))
    {}

    Clues::Data Clues::computeClues ( Array2D<bool> const& data
                                    , Type type
                                    )
    {
      auto const columns
        (type == Type::Column ? data.rows().value : data.columns().value);
      auto const rows
        (type == Type::Column ? data.columns().value : data.rows().value);

      std::vector<std::vector<Value>> clues;

      clues.resize (rows);

      auto is_filled
        ( [&] (Column column, Row row)
          {
            return type == Type::Column
                ? data.at ({Column {row.value}, Row {column.value}})
                : data.at ({column, row})
                ;
          }
        );

      for (Row row {0}; row.value < rows; ++row.value)
      {
        unsigned int filled_counter (0);
        for (Column column {0}; column.value < columns; ++column.value)
        {
          bool const current_square_filled (is_filled (column, row));

          if (current_square_filled)
          {
            filled_counter++;
          }

          if ( column.value > 0
            && !current_square_filled
            && is_filled (Column {column.value - 1}, row)
             )
          {
            clues[row.value].push_back (filled_counter);
            filled_counter = 0;
          }

          if ( column.value == columns - 1
            && filled_counter > 0
             )
          {
            clues[row.value].push_back (filled_counter);
          }
        }

        max_minor_size_.value = std::max ( max_minor_size_.value
                                         , clues[row.value].size()
                                         );
      }

      return Data (clues);
    }

    MinorSize Clues::maxNumberOfClues() const
    {
      return max_minor_size_;
    }

    MainSize Clues::mainSize() const
    {
      return data_.mainSize();
    }

    MinorSize Clues::minorSize (MainIndex main_index) const
    {
      return data_.minorSize (main_index);
    }

    Clues::Value Clues::clue (FullIndex full_index) const
    {
      return data_.at (full_index);
    }
  }
}
