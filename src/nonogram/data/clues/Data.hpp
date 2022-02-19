#pragma once

#include <nonogram/data/clues/FullIndex.hpp>
#include <nonogram/data/clues/FullIndices.hpp>
#include <nonogram/data/clues/MainIndex.hpp>
#include <nonogram/data/clues/MinorIndex.hpp>

#include <QtCore/QDataStream>

#include <algorithm>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <vector>
#include <utility>

namespace nonogram::data::clues
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

    MainIndex mainSize() const
    {
      return MainIndex (data_.size());
    }

    MinorIndex minorSize (MainIndex main_index) const
    {
      return MinorIndex (data_.at (main_index.value).size());
    }

    T at (MainIndex main_index, MinorIndex minor_index) const
    {
      checkInvalidAccess (main_index, minor_index);

      return data_.at (main_index.value).at (minor_index.value);
    }

    T at (FullIndex full_index) const
    {
      return at (full_index.main, full_index.minor);
    }

    bool any_of (std::function<bool (FullIndex, T)> check) const
    {
      for (auto const& [full_index, value] : *this)
      {
        if (check (full_index, value))
        {
          return true;
        }
      }

      return false;
    }

    FullIndices indices_if (std::function<bool (FullIndex, T)> check) const
    {
      FullIndices indices;

      for (auto const& [full_index, value] : *this)
      {
        if (check (full_index, value))
        {
          indices.insert (full_index);
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

      data_.at (main_index.value).at (minor_index.value) = value;
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
        return { full_index
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
        std::size_t counter (0);
        for (MainIndex main (0); main < MainIndex (data_.size()); ++main)
        {
          if (index < (counter + data_.at (main.value).size()))
          {
            return {main, MinorIndex (index - counter)};
          }

          counter += data_.at (main.value).size();
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
          data_.at (main.value).at (minor.value) = value;

          ++count_;
        }
      }
    }
    friend QDataStream& operator<< (QDataStream& ds, Data const& data)
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
