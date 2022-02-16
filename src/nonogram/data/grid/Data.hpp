#pragma once

#include <nonogram/data/grid/Cell.hpp>
#include <nonogram/data/grid/Cells.hpp>
#include <nonogram/data/grid/Column.hpp>
#include <nonogram/data/grid/Row.hpp>

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
    namespace grid
    {
      template<typename T>
        class Data
      {
      public:
        Data (Column columns, Row rows, std::vector<T> data)
        : rows_ (rows)
        , columns_ (columns)
        , data_ (std::move (data))
        {}

        Data (Column columns, Row rows, T value = T())
        : Data ( columns
               , rows
               , std::vector<T> (columns.value * rows.value, value)
               )
        {}

        Data (Data const&) = default;

        Row rows() const
        {
          return rows_;
        }

        Column columns() const
        {
          return columns_;
        }

        using const_reference = typename std::vector<T>::const_reference;

        const_reference at (Column column, Row row) const
        {
          checkInvalidAccess (column, row);

          return data_.at (row.value * columns_.value + column.value);
        }

        const_reference at (Cell cell) const
        {
          return at (cell.column, cell.row);
        }

        using reference = typename std::vector<T>::reference;

        reference at (Column column, Row row)
        {
          checkInvalidAccess (column, row);

          return data_.at (row.value * columns_.value + column.value);
        }

        reference at (Cell cell)
        {
          return at (cell.column, cell.row);
        }

        bool any_of (std::function<bool (Cell, T)> check) const
        {
          for (Row row {0}; row < rows(); ++row)
          {
            for (Column column {0}; column < columns(); ++column)
            {
              Cell const cell {column, row};
              if (check (cell, at (cell)))
              {
                return true;
              }
            }
          }

          return false;
        }

        Cells cells_if (std::function<bool (Cell, T)> check) const
        {
          Cells data;

          for (Row row {0}; row < rows(); ++row)
          {
            for (Column column {0}; column < columns(); ++column)
            {
              Cell const cell {column, row};
              if (check (cell, at (cell)))
              {
                data.insert (cell);
              }
            }
          }

          return data;
        }

        void fill (T value)
        {
          std::fill (data_.begin(), data_.end(), value);
        }

        void set (Column column, Row row, T value)
        {
          checkInvalidAccess (column, row);

          at (column, row) = value;
        }

        void set (Cell cell, T value)
        {
          set (cell.column, cell.row, value);
        }

        // serialization
        Data (QDataStream& ds)
        : rows_ (ds)
        , columns_ (ds)
        , data_ (rows_.value * columns_.value)
        {
          for (Row row {0}; row < rows_; ++row)
          {
            for (Column column {0}; column < columns_; ++column)
            {
              T value;
              ds >> value;
              set (column, row, value);
            }
          }
        }
        friend QDataStream& operator<< (QDataStream& ds, Data const& array)
        {
          auto const rows (array.rows());
          auto const columns (array.columns());
          ds << rows.value;
          ds << columns.value;

          for (Row row {0}; row < rows; ++row)
          {
            for (Column column {0}; column < columns; ++column)
            {
              ds << array.at (column, row);
            }
          }

          return ds;
        }

      private:
        void checkInvalidAccess (Column column) const
        {
          if (column >= columns_)
          {
            throw std::invalid_argument
              ("Invalid access to column " + column.toString());
          }
        }
        void checkInvalidAccess (Row row) const
        {
          if (row >= rows_)
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

        Row rows_;
        Column columns_;
        std::vector<T> data_;
      };
    }
  }
}
