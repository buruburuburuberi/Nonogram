#include <nonogram/data/Solution.hpp>

#include <iostream>
#include <string>

namespace nonogram
{
  namespace data
  {
    Solution::Solution (Data data)
    : data_ (std::move (data))
    {
      for (auto const& type : Clues::all_types)
      {
        clues_.emplace
          ( std::piecewise_construct
          , std::forward_as_tuple (type)
          , std::forward_as_tuple (data_, type)
          );
      }
    }

    Row Solution::dataRows() const
    {
      return data_.rows();
    }

    Column Solution::dataColumns() const
    {
      return data_.columns();
    }

    Solution::State Solution::at (Slot slot) const
    {
      return data_.at (slot);
    }

    MinorIndex Solution::maxNumberOfClues (Clues::Type type) const
    {
      return clues_.at (type).maxNumberOfClues();
    }

    MainIndex Solution::clueMainSize (Clues::Type type) const
    {
      return clues_.at (type).mainSize();
    }

    MinorIndex Solution::clueMinorSize (Clues::Type type, MainIndex main_index) const
    {
      return clues_.at (type).minorSize (main_index);
    }

    Clues::Value Solution::clue (Clues::Type type, FullIndex full_index) const
    {
      return clues_.at (type).clue (full_index);
    }
  }
}
