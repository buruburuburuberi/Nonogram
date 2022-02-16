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
        using Container = std::vector<std::vector<T>>;

        Data (Container data)
        : data_ (data)
        , count_ ( std::accumulate
                     ( data_.begin()
                     , data_.end()
                     , 0
                     , [] (int acc, std::vector<T> const& row)
                       {
                         return acc + row.size();
                       }
                     )
                 )
        {}

        Data (Data const& data)
        : data_ (data.data_)
        , count_ (data.count_)
        {}

        MainIndex mainSize() const
        {
          return MainIndex (data_.size());
        }

        MinorIndex minorSize (MainIndex main_index) const
        {
          return MinorIndex (data_.at (main_index.value).size());
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

          for (auto const& data : *this)
          {
            if (check (data.first, data.second))
            {
              indices.insert (data.first);
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

        struct Iterator
        {
          constexpr bool operator!= (Iterator const& other) const
          {
            return index_ != other.index_;
          }

          constexpr Iterator& operator++()
          {
            ++index_;
            return *this;
          }

          constexpr std::pair<FullIndex, T> operator*()
          {
            auto const full_index (fromIndex (index_));
            return std::pair<FullIndex, T>
                { full_index
                , data_.at (full_index.main.value).at (full_index.minor.value)
                };
          }

        private:
          Iterator (Container const& data, std::size_t index)
          : data_ (data)
          , index_ (index)
          {}

          FullIndex fromIndex (std::size_t index) const
          {
            for (std::size_t row (0), counter (0); row < data_.size(); ++row)
            {
              if (index < (counter + data_.at (row).size()))
              {
                return { MainIndex (row), MinorIndex (index - counter) };
              }

              counter += data_.at (row).size();
            }

            throw std::invalid_argument ("Invalid access to index " + index);
          }

          Container const& data_;
          size_t index_;

          friend Data<T>;
        };

        constexpr Iterator begin() const
        {
          return Iterator (data_, 0);
        }

        constexpr Iterator end() const
        {
          return Iterator (data_, count_);
        }

        // serialization
        Data (QDataStream& ds)
        : data_ (MainIndex (ds).value)
        , count_ (0)
        {
          for (MainIndex main {0}; main < MainIndex (data_.size()); ++main)
          {
            MinorIndex const minor_size (ds);
            data_.at (main.value).resize (minor_size.value);

            for (MinorIndex minor {0}; minor < minor_size; ++minor)
            {
              T value;
              ds >> value;
              data_[main.value][minor.value] = value;

              ++count_;
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
              ("Invalid access to minor_index " + minor_index.toString());
          }
        }

        Container data_;
        std::size_t count_;
      };
    }
  }
}
