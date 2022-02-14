#pragma once

#include <nonogram/data/Array2D.hpp>
#include <nonogram/util/hard_integral_typedef.hpp>

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
    HARD_INTEGRAL_TYPEDEF (MainIndex, std::size_t);
    HARD_INTEGRAL_TYPEDEF (MinorIndex, std::size_t);

    struct FullIndex
    {
      MainIndex main;
      MinorIndex minor;

      bool operator== (FullIndex const& rhs) const
      {
        return std::tie (main, minor) == std::tie (rhs.main, rhs.minor);
      }

      bool operator< (FullIndex const& rhs) const
      {
        return std::tie (main, minor) < std::tie (rhs.main, rhs.minor);
      }
    };

    using Indices = std::set<data::FullIndex>;

    template<typename T>
    class VectorOfVectors
    {
    public:
      VectorOfVectors (MainIndex size)
      {
        data_.resize (size.value);
      }

      VectorOfVectors (std::vector<std::vector<T>> data)
      : data_ (data)
      {}

      VectorOfVectors (VectorOfVectors const& data)
      : data_ (data.data_)
      {}

      MainIndex mainSize() const
      {
        return MainIndex (data_.size());
      }

      MinorIndex minorSize (MainIndex main_index) const
      {
        return MinorIndex (data_.at (main_index.value).size());
      }

      void resize (MainIndex main_index, MinorIndex minor_size, T value)
      {
        data_.at (main_index.value).resize (minor_size.value, value);
      }

      bool has (MainIndex main_index, MinorIndex minor_index) const
      {
        return main_index < mainSize() && minor_index < minorSize (main_index);
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

        for (MainIndex main_index {0}; main_index < mainSize(); ++main_index)
        {
          for ( MinorIndex minor_index {0}
              ; minor_index < minorSize (main_index)
              ; ++minor_index
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
        if (index >= mainSize())
        {
          throw std::invalid_argument
            ("Invalid access to main_index " + index.toString());
        }
      }
      void checkInvalidAccess (MainIndex main_index, MinorIndex minor_index) const
      {
        checkInvalidAccess (main_index);

        if (minor_index >= minorSize (main_index))
        {
          throw std::invalid_argument
            ("Invalid access to column " + minor_index.toString());
        }
      }

      std::vector<std::vector<T>> data_;
    };
  }
}
