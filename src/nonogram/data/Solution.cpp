#include <nonogram/data/Solution.hpp>

#include <iostream>
#include <string>

namespace nonogram
{
  namespace data
  {
    Solution::Solution (Array2D<bool> data)
    : data_ (std::move (data))
    {
      clues_.emplace (ClueType::Right, std::move (compute_right_clues()));
      clues_.emplace (ClueType::Bottom, std::move (compute_bottom_clues()));
      clues_.emplace
        ( ClueType::Left
        , std::move (compute_left_clues (clues_.at (ClueType::Right)))
        );
      clues_.emplace
        ( ClueType::Top
        , std::move (compute_top_clues (clues_.at (ClueType::Bottom)))
        );
    }

    Solution::Clues Solution::compute_right_clues() const
    {
      std::vector<std::vector<Clue>> clues;
      clues.resize (data_.rows().value);

      std::size_t max_size (0);

      for (Row row {0}; row.value < rows_of_data().value; ++row.value)
      {
        unsigned int horizontal_filled_counter (0);
        for ( Column column {0}
            ; column.value < columns_of_data().value
            ; ++column.value
            )
        {
          bool const current_square_filled (data_.at (column, row));

          if (current_square_filled)
          {
            horizontal_filled_counter++;
          }

          if ( column.value > 0
            && !current_square_filled
            && data_.at (Column {column.value - 1}, row)
             )
          {
            clues[row.value].push_back (horizontal_filled_counter);
            horizontal_filled_counter = 0;
          }

          if ( column.value == data_.columns().value - 1
            && horizontal_filled_counter > 0
             )
          {
            clues[row.value].push_back (horizontal_filled_counter);
          }
        }

        max_size = std::max (max_size, clues[row.value].size());
      }

      for (auto& row : clues)
      {
        row.resize (max_size, 0);
      }

      return Clues (clues);
    }

    Solution::Clues Solution::compute_bottom_clues() const
    {
      std::vector<std::vector<Clue>> clues;

      std::size_t max_size (0);

      for ( Column column {0}
          ; column.value < columns_of_data().value
          ; ++column.value
          )
      {
        unsigned int row_counter (0);
        unsigned int vertical_filled_counter (0);
        for (Row row {0}; row.value < rows_of_data().value; ++row.value)
        {
          bool const current_square_filled (data_.at (column, row));

          if (current_square_filled)
          {
            vertical_filled_counter++;
          }

          if ( row.value > 0
            && !current_square_filled
            && data_.at (column, Row {row.value - 1})
             )
          {
            if (row_counter == clues.size())
            {
              clues.push_back
                (std::vector<Clue> (data_.columns().value, 0));
            }

            clues[row_counter][column.value] = vertical_filled_counter;
            row_counter++;
            vertical_filled_counter = 0;
          }

          if ( row.value == data_.rows().value - 1
            && vertical_filled_counter > 0
             )
          {
            if (row_counter == clues.size())
            {
              clues.push_back
                (std::vector<Clue> (data_.columns().value, 0));
            }

            clues[row_counter][column.value] = vertical_filled_counter;
            row_counter++;
          }
        }

        max_size = std::max (max_size, clues[column.value].size());
      }

      return Clues (clues);
    }

    Solution::Clues Solution::compute_left_clues (Clues const& data) const
    {
      Clues clues (data);

      for (Row row {0}; row.value < data.rows().value; ++row.value)
      {
        auto clues_of_row (data.row (row));
        if (clues_of_row.back() > 0)
        {
          continue;
        }

        std::vector<Clue> new_clues;

        for (auto const& clue : clues_of_row)
        {
          if (clue > 0)
          {
            new_clues.push_back (clue);
          }
        }

        auto const number_of_zeroes (clues_of_row.size() - new_clues.size());
        new_clues.insert (new_clues.begin(), number_of_zeroes, 0);

        for (Column column {0}; column.value < new_clues.size(); ++column.value)
        {
          clues.set (column, row, new_clues.at (column.value));
        }
      }

      return clues;
    }

    Solution::Clues Solution::compute_top_clues (Clues const& data) const
    {
      Clues clues (data);

      for (Column column {0}; column.value < data.columns().value; ++column.value)
      {
        auto const clues_of_column (clues.column (column));
        if (clues_of_column.back() > 0)
        {
          continue;
        }

        std::vector<Clue> new_clues;

        for (auto const& clue : clues_of_column)
        {
          if (clue > 0)
          {
            new_clues.push_back (clue);
          }
        }

        auto const number_of_zeroes (clues_of_column.size() - new_clues.size());
        new_clues.insert (new_clues.begin(), number_of_zeroes, 0);

        for (Row row {0}; row.value < new_clues.size(); ++row.value)
        {
          clues.set (column, row, new_clues.at (row.value));
        }
      }

      return clues;
    }

    Rows Solution::rows_of_data() const
    {
      return data_.rows();
    }

    Columns Solution::columns_of_data() const
    {
      return data_.columns();
    }

    Solution::State Solution::at (Slot slot) const
    {
      return data_.at (slot);
    }

    Columns Solution::columns_of_clues (ClueType type) const
    {
      return clues_.at (type).columns();
    }

    Rows Solution::rows_of_clues (ClueType type) const
    {
      return clues_.at (type).rows();
    }

    Solution::Clue Solution::clue (ClueType type, Slot slot) const
    {
      return clues_.at (type).at (slot);
    }
  }
}
