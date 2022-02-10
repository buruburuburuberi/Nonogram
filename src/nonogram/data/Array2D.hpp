#pragma once

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
    struct Column { std::size_t value; };
    struct Columns { std::size_t value; };
    struct Row { std::size_t value; };
    struct Rows { std::size_t value; };

    struct Slot
    {
      Column column;
      Row row;

      bool operator== (Slot const& rhs) const
      {
        return std::tie (column.value, row.value)
            == std::tie (rhs.column.value, rhs.row.value);
      }

      bool operator< (Slot const& rhs) const
      {
        if (row.value < rhs.row.value)
        {
          return true;
        }
        else if (row.value == rhs.row.value)
        {
          return column.value < rhs.column.value;
        }
        else
        {
          return false;
        }
      }
    };

    using Slots = std::set<data::Slot>;

    template<typename T>
    class Array2D
    {
    public:
      Array2D (Columns columns, Rows rows, T value = T())
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
        checkInvalidAccess (column, row);

        return data_[row.value][column.value];
      }

      T at (Slot slot) const
      {
        return at (slot.column, slot.row);
      }

      Slots slots_if (std::function<bool (Slot, T)> check) const
      {
        Slots slots;

        for (Row row {0}; row.value < rows().value; ++row.value)
        {
          for (Column column {0}; column.value < columns().value; ++column.value)
          {
            Slot const slot {column, row};
            if (check (slot, at (slot)))
            {
              slots.insert (slot);
            }
          }
        }

        return slots;
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

    private:
      void checkInvalidAccess (Column column) const
      {
        if (column.value >= columns().value)
        {
          throw std::invalid_argument
            ("Invalid access to column " + std::to_string (column.value));
        }
      }
      void checkInvalidAccess (Row row) const
      {
        if (row.value >= rows().value)
        {
          throw std::invalid_argument
            ("Invalid access to row " + std::to_string (row.value));
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
