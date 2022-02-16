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
        using Container = std::vector<T>;

        Data (Column columns, Row rows, Container data)
        : rows_ (rows)
        , columns_ (columns)
        , data_ (std::move (data))
        {}

        Data (Column columns, Row rows, T value = T())
        : Data ( columns
               , rows
               , Container (columns.value * rows.value, value)
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

        using ConstReference = typename Container::const_reference;

        ConstReference at (Column column, Row row) const
        {
          checkInvalidAccess (column, row);

          return data_.at (row.value * columns_.value + column.value);
        }

        ConstReference at (Cell cell) const
        {
          return at (cell.column, cell.row);
        }

        using reference = typename Container::reference;

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
          for (auto const& cell : *this)
          {
            if (check (cell.first, cell.second))
            {
              return true;
            }
          }

          return false;
        }

        Cells cells_if (std::function<bool (Cell, T)> check) const
        {
          Cells data;

          for (auto const& cell : *this)
          {
            if (check (cell.first, cell.second))
            {
              data.insert (cell.first);
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

        using ConstIterator = typename Container::const_iterator;

        struct Iterator
        {
          constexpr bool operator!= (ConstIterator const& other) const
          {
            return value_ != other;
          }

          constexpr Iterator& operator++()
          {
            ++index_;
            ++value_;
            return *this;
          }

          constexpr std::pair<Cell, ConstReference> operator*()
          {
            return std::pair<Cell, ConstReference>
              {fromIndex (index_), *value_};
          }

        private:
          Iterator (Row rows, Column columns, ConstIterator value)
          : rows_ (rows)
          , columns_ (columns)
          , index_ (0)
          , value_ (value)
          {}

          Cell fromIndex (std::size_t index) const
          {
            return { Column {index % columns_.value}
                   , Row {index / columns_.value}
                   };
          }

          Row rows_;
          Column columns_;
          size_t index_;
          ConstIterator value_;

          friend Data<T>;
        };

        constexpr Iterator begin() const
        {
          return Iterator (rows_, columns_, data_.begin());
        }

        constexpr ConstIterator end() const
        {
          return data_.end();
        }

        // serialization
        Data (QDataStream& ds)
        : rows_ (ds)
        , columns_ (ds)
        , data_ (rows_.value * columns_.value)
        {
          for (auto const& cell : *this)
          {
            T value;
            ds >> value;
            set (cell.first, value);
          }
        }
        friend QDataStream& operator<< (QDataStream& ds, Data const& array)
        {
          auto const rows (array.rows());
          auto const columns (array.columns());
          ds << rows.value;
          ds << columns.value;

          for (auto const& cell : array)
          {
            ds << cell.second;
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
        Container data_;
      };
    }
  }
}
