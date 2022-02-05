#pragma once

#include <nonogram/data/Array2D.hpp>

#include <algorithm>
#include <cstddef>
#include <functional>
#include <set>
#include <stdexcept>
#include <vector>

namespace nonogram
{
  namespace data
  {
    struct MainIndex { std::size_t value; };
    struct MainSize { std::size_t value; };
    struct MinorIndex { std::size_t value; };
    struct MinorSize { std::size_t value; };

    struct FullIndex
    {
      MainIndex main;
      MinorIndex minor;

      bool operator== (FullIndex const& rhs) const
      {
        return minor.value == rhs.minor.value
            && main.value == rhs.main.value;
      }

      bool operator< (FullIndex const& rhs) const
      {
        if (main.value < rhs.main.value)
        {
          return true;
        }
        else if (main.value > rhs.main.value)
        {
          return false;
        }
        else
        {
          return minor.value < rhs.minor.value;
        }
      }
    };

    using Indices = std::set<data::FullIndex>;

    template<typename T>
    class VectorOfVectors
    {
    public:
      VectorOfVectors (MainSize size)
      {
        data_.resize (size.value);
      }

      VectorOfVectors (std::vector<std::vector<T>> data)
      : data_ (data)
      {}

      VectorOfVectors (VectorOfVectors const& data)
      : data_ (data.data_)
      {}

      MainSize mainSize() const
      {
        return {data_.size()};
      }

      MinorSize minorSize (MainIndex main_index) const
      {
        return {data_.at (main_index.value).size()};
      }

      void resize (MainIndex main_index, MinorSize minor_size, T value)
      {
        data_.at (main_index.value).resize (minor_size.value, value);
      }

      bool has (MainIndex main_index, MinorIndex minor_index) const
      {
        return main_index.value < data_.size()
            && minor_index.value < data_.at (main_index.value).size();
      }

      bool has (FullIndex full_index) const
      {
        return has (full_index.main, full_index.minor);
      }

      T at (MainIndex main_index, MinorIndex minor_index) const
      {
        checkInvalidAccess (main_index, minor_index);

        return data_[main_index.value][minor_index.value];
      }

      T at (FullIndex full_index) const
      {
        return at (full_index.main, full_index.minor);
      }

      Indices indices_if (std::function<bool (FullIndex, T)> check) const
      {
        Indices indices;

        for ( MainIndex main_index {0}
            ; main_index.value < data_.size()
            ; ++main_index.value
            )
        {
          for ( MinorIndex minor_index {0}
              ; minor_index.value < data_.at (main_index.value).size()
              ; ++minor_index.value
              )
          {
            FullIndex const full_index {main_index, minor_index};
            if (check (full_index, at (full_index)))
            {
              indices.insert (full_index);
            }
          }
        }

        return indices;
      }

      void fill (T value)
      {
        for (auto& row : data_)
        {
          std::fill (row.begin(), row.end(), value);
        }
      }

      void set (MainIndex main_index, MinorIndex minor_index, T value)
      {
        checkInvalidAccess (main_index, minor_index);

        data_[main_index.value][minor_index.value] = value;
      }

      void set (FullIndex full_index, T value)
      {
        set (full_index.main, full_index.minor, value);
      }

    private:
      void checkInvalidAccess (MainIndex index) const
      {
        if (index.value >= mainSize().value)
        {
          throw std::invalid_argument
            ("Invalid access to main_index " + std::to_string (index.value));
        }
      }
      void checkInvalidAccess (MainIndex main_index, MinorIndex minor_index) const
      {
        checkInvalidAccess (main_index);

        if (minor_index.value >= minorSize (main_index).value)
        {
          throw std::invalid_argument
            ("Invalid access to column " + std::to_string (minor_index.value));
        }
      }

      std::vector<std::vector<T>> data_;
    };
  }
}
