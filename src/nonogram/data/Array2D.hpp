#pragma once

#include <nonogram/util/hard_integral_typedef.hpp>

#include <QtCore/QDataStream>

#include <algorithm>
#include <cstddef>
#include <functional>
#include <set>
#include <stdexcept>
#include <tuple>
#include <vector>

namespace nonogram
{
  namespace data
  {
    HARD_INTEGRAL_TYPEDEF (Column, unsigned long long int);
    HARD_INTEGRAL_TYPEDEF (Row, unsigned long long int);

    struct Slot
    {
      Column column;
      Row row;

      bool operator== (Slot const& rhs) const
      {
        return std::tie (column, row) == std::tie (rhs.column, rhs.row);
      }

      bool operator< (Slot const& rhs) const
      {
        return std::tie (row, column) < std::tie (rhs.row, rhs.column);
      }
    };

    using Slots = std::set<data::Slot>;

    template<typename T>
    class Array2D
    {
    public:
      Array2D (Column columns, Row rows, T value = T())
      {
        data_.resize (rows.value);
        for (auto& row : data_)
        {
          row.resize (columns.value, value);
        }
      }

      Array2D (std::vector<std::vector<T>> data)
      : data_ (data)
      {}

      Array2D (Array2D const&) = default;

      Row rows() const
      {
        return Row {data_.size()};
      }

      Column columns() const
      {
        if (data_.empty())
        {
          return Column {0};
        }

        return Column {data_.front().size()};
      }

      T at (Column column, Row row) const
      {
        checkInvalidAccess (column, row);

        return data_[row.value][column.value];
      }

      T at (Slot slot) const
      {
        return at (slot.column, slot.row);
      }

      Slots slots_if (std::function<bool (Slot, T)> check) const
      {
        Slots data;

        for (Row row {0}; row < rows(); ++row)
        {
          for (Column column {0}; column < columns(); ++column)
          {
            Slot const slot {column, row};
            if (check (slot, at (slot)))
            {
              data.insert (slot);
            }
          }
        }

        return data;
      }

      std::vector<T> column (Column column) const
      {
        checkInvalidAccess (column);

        std::vector<T> data;

        for (auto const& row : data_)
        {
          data.push_back (row.at (column.value));
        }

        return data;
      }

      std::vector<T> row (Row row) const
      {
        checkInvalidAccess (row);

        return data_.at (row.value);
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
        checkInvalidAccess (column, row);

        data_[row.value][column.value] = value;
      }

      void set (Slot slot, T value)
      {
        set (slot.column, slot.row, value);
      }

      // serialization
      Array2D (QDataStream& ds)
      {
        Row::underlying_type rows_value;
        ds >> rows_value;
        Row rows {rows_value};
        data_.resize (rows.value);

        Column::underlying_type columns_value;
        ds >> columns_value;
        Column columns {columns_value};

        for (Row row {0}; row < rows; ++row)
        {
          data_.at (row.value).resize (columns.value);

          for (Column column {0}; column < columns; ++column)
          {
            T value;
            ds >> value;
            data_[row.value][column.value] = value;
          }
        }
      }
      friend QDataStream& operator<< (QDataStream& ds, Array2D const& array)
      {
        auto const rows (array.rows());
        auto const columns (array.columns());
        ds << rows.value;
        ds << columns.value;

        for (Row row {0}; row < rows; ++row)
        {
          for (Column column {0}; column < columns; ++column)
          {
            ds << array.data_.at (row.value).at (column.value);
          }
        }

        return ds;
      }

    private:
      void checkInvalidAccess (Column column) const
      {
        if (column >= columns())
        {
          throw std::invalid_argument
            ("Invalid access to column " + column.toString());
        }
      }
      void checkInvalidAccess (Row row) const
      {
        if (row >= rows())
        {
          throw std::invalid_argument
            ("Invalid access to row " + row.toString());
        }
      }
      void checkInvalidAccess (Column column, Row row) const
      {
        checkInvalidAccess (column);
        checkInvalidAccess (row);
      }

      std::vector<std::vector<T>> data_;
    };
  }
}
