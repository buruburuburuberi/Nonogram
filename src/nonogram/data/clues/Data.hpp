#pragma once

#include <nonogram/data/clues/FullIndex.hpp>
#include <nonogram/data/clues/FullIndices.hpp>
#include <nonogram/data/clues/MainIndex.hpp>
#include <nonogram/data/clues/MinorIndex.hpp>

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
    namespace clues
    {
      template<typename T>
        class Data
      {
      public:
        Data (MainIndex size)
        {
          data_.resize (size.value);
        }

        Data (std::vector<std::vector<T>> data)
        : data_ (data)
        {}

        Data (Data const& data)
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

        FullIndices indices_if (std::function<bool (FullIndex, T)> check) const
        {
          FullIndices indices;

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

        // serialization
        Data (QDataStream& ds)
        {
          MainIndex const main_size (ds);
          data_.resize (main_size.value);

          for (MainIndex main {0}; main < main_size; ++main)
          {
            MinorIndex const minor_size (ds);
            data_.at (main.value).resize (minor_size.value);

            for (MinorIndex minor {0}; minor < minor_size; ++minor)
            {
              T value;
              ds >> value;
              data_[main.value][minor.value] = value;
            }
          }
        }
        friend QDataStream& operator<< ( QDataStream& ds
                                       , Data const& data
                                       )
        {
          auto const main_size (data.mainSize());
          ds << main_size.value;

          for (MainIndex main {0}; main < main_size; ++main)
          {
            auto const minor_size (data.minorSize (main));
            ds << minor_size.value;

            for (MinorIndex minor {0}; minor < minor_size; ++minor)
            {
              ds << data.at (main, minor);
            }
          }

          return ds;
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
}
