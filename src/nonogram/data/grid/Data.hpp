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
        Data (Column columns, Row rows, T value = T())
        {
          data_.resize (rows.value);
          for (auto& row : data_)
          {
            row.resize (columns.value, value);
          }
        }

        Data (std::vector<std::vector<T>> data)
        : data_ (data)
        {}

        Data (Data const&) = default;

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

        T at (Cell cell) const
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

        void set (Cell cell, T value)
        {
          set (cell.column, cell.row, value);
        }

        // serialization
        Data (QDataStream& ds)
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
}
