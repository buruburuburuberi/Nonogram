#include <nonogram/data/Clues.hpp>

#include <algorithm>
#include <stdexcept>

namespace nonogram
{
  namespace data
  {
    clues::FullIndex Clues::toFullIndex (Type type, grid::Cell cell)
    {
      return
        { clues::MainIndex
            {type == Type::Column ?  cell.column.value : cell.row.value}
        , clues::MinorIndex
            {type == Type::Column ? cell.row.value : cell.column.value}
        };
    }

    Clues::Clues (grid::Data<bool> const& data, Type type)
    : type_ (type)
    , max_minor_size_ {0}
    , data_ (computeClues (data, type))
    {}

    Clues::Data Clues::computeClues ( grid::Data<bool> const& data
                                    , Type type
                                    )
    {
      grid::Column const columns
        (type == Type::Column ? data.rows().value : data.columns().value);
      grid::Row const rows
        (type == Type::Column ? data.columns().value : data.rows().value);

      std::vector<std::vector<Value>> clues;

      clues.resize (rows.value);

      auto is_filled
        ( [&] (grid::Column column, grid::Row row)
          {
            return type == Type::Column
                ? data.at ({grid::Column {row.value}, grid::Row {column.value}})
                : data.at ({column, row})
                ;
          }
        );

      for (grid::Row row {0}; row < rows; ++row)
      {
        unsigned int filled_counter (0);
        for (grid::Column column {0}; column < columns; ++column)
        {
          bool const current_square_filled (is_filled (column, row));

          if (current_square_filled)
          {
            filled_counter++;
          }

          if ( column > grid::Column (0)
            && !current_square_filled
            && is_filled (grid::Column {column - grid::Column (1)}, row)
             )
          {
            clues.at (row.value).push_back (filled_counter);
            filled_counter = 0;
          }

          if ( column == columns - grid::Column (1)
            && filled_counter > 0
             )
          {
            clues.at (row.value).push_back (filled_counter);
          }
        }

        max_minor_size_.value =
            std::max ( max_minor_size_.value
                     , static_cast<clues::MinorIndex::underlying_type>
                        (clues.at (row.value).size())
                     );
      }

      return Data (clues);
    }

    clues::MinorIndex Clues::maxNumberOfClues() const
    {
      return max_minor_size_;
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
