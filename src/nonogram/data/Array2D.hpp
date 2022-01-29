#pragma once

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace nonogram
{
  namespace data
  {
    struct Column { std::size_t value; };
    struct Columns { std::size_t value; };
    struct Row { std::size_t value; };
    struct Rows { std::size_t value; };

    template<typename T>
    class Array2D
    {
    public:
      Array2D() = default;

      Array2D (Columns columns, Rows rows)
      {
        data_.resize (rows.value);
        for (auto& row : data_)
        {
          row.resize (columns.value);
        }
      }

      Array2D (std::vector<std::vector<T>> data)
      : data_ (data)
      {}

      Array2D (Array2D const& data)
      : data_ (data.data_)
      {}

      Rows rows() const
      {
        return Rows {data_.size()};
      }

      Columns columns() const
      {
        if (data_.empty())
        {
          return Columns {0};
        }

        return Columns {data_.front().size()};
      }

      T at (Column column, Row row) const
      {
        check_invalid_access (column, row);

        return data_[row.value][column.value];
      }

      std::vector<T> column (Column column)
      {
        check_invalid_access (column);

        std::vector<T> data;


        return data;
      }

      std::vector<T> row (Row row)
      {
        check_invalid_access (column);

        return data_.at (row);
      }

      void fill (T value)
      {
        for (auto& row : data_)
        {
          std::fill (row.begin(), row.end(), value);
        }
      }

      void set (Column column, Row row, T value)
      {
        check_invalid_access (column, row);

        data_[row.value][column.value] = value;
      }

    private:
      void check_invalid_access (Column column) const
      {
        if (column.value >= columns().value)
        {
          throw std::invalid_argument
            ("Invalid access to column " + std::to_string (column.value));
        }
      }
      void check_invalid_access (Row row) const
      {
        if (row.value >= rows().value)
        {
          throw std::invalid_argument
            ("Invalid access to row " + std::to_string (row.value));
        }
      }
      void check_invalid_access (Column column, Row row) const
      {
        check_invalid_access (column);
        check_invalid_access (row);
      }

      std::vector<std::vector<T>> data_;
    };
  }
}
