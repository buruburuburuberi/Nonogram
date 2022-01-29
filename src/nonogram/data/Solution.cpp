#include <nonogram/data/Solution.hpp>

#include <string>

namespace nonogram
{
  namespace data
  {
    Solution::Solution (Array2D<bool> data)
    : data_ (std::move (data))
    , size_of_clues_ (0, 0)
    {
      clues_of_columns_.resize (data_.columns().value);
      clues_of_rows_.resize (data_.rows().value);

      for (Column column {0}; column.value < columns().value; ++column.value)
      {
        unsigned int horizontal_filled_counter (0);
        for (Row row {0}; row.value < rows().value; ++row.value)
        {
          bool const current_square_filled (data_.at (column, row));

          if (current_square_filled)
          {
            horizontal_filled_counter++;
          }

          if ( row.value > 0
            && !current_square_filled
            && data_.at (column, Row {row.value - 1})
             )
          {
            clues_of_columns_[column.value].push_back (horizontal_filled_counter);
            horizontal_filled_counter = 0;
          }

          if ( row.value == data_.rows().value - 1
            && horizontal_filled_counter > 0
             )
          {
            clues_of_columns_[column.value].push_back (horizontal_filled_counter);
          }
        }

        size_of_clues_.setHeight
          ( std::max ( static_cast<std::size_t> (size_of_clues_.height())
                     , clues_of_columns_[column.value].size()
                     )
          );
      }

      for (Row row {0}; row.value < rows().value; ++row.value)
      {
        unsigned int vertical_filled_counter (0);
        for (Column column {0}; column.value < columns().value; ++column.value)
        {
          bool const current_square_filled (data_.at (column, row));

          if (current_square_filled)
          {
            vertical_filled_counter++;
          }

          if ( column.value > 0
            && !current_square_filled
            && data_.at (Column {column.value - 1}, row)
             )
          {
            clues_of_rows_[row.value].push_back (vertical_filled_counter);
            vertical_filled_counter = 0;
          }

          if ( column.value == data_.columns().value - 1
            && vertical_filled_counter > 0
             )
          {
            clues_of_rows_[row.value].push_back (vertical_filled_counter);
          }
        }

        size_of_clues_.setWidth
          ( std::max ( static_cast<std::size_t> (size_of_clues_.width())
                     , clues_of_rows_[row.value].size()
                     )
          );
      }
    }

    Rows Solution::rows() const
    {
      return data_.rows();
    }

    Columns Solution::columns() const
    {
      return data_.columns();
    }

    QSize Solution::size_of_clues() const
    {
      return size_of_clues_;
    }

    Solution::Clues const& Solution::clues (Column column) const
    {
      if (column.value >= columns().value)
      {
        throw std::invalid_argument
          ("Invalid access (" + std::to_string (column.value) + ")");
      }

      return clues_of_columns_.at (column.value);
    }

    Solution::Clues const& Solution::clues (Row row) const
    {
      if (row.value >= rows().value)
      {
        throw std::invalid_argument
          ("Invalid access (" + std::to_string (row.value) + ")");
      }

      return clues_of_rows_.at (row.value);
    }
  }
}
