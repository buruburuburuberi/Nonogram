#include <nonogram/data/Nonogram.hpp>

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

namespace nonogram
{
  namespace data
  {
    Nonogram::Nonogram (std::string name, Solution data)
    : name_ (std::move (name))
    , solution_ (std::move (data))
    {
      std::fill (current_.begin(), current_.end(), Type::Empty);
    }

    std::string Nonogram::name() const
    {
      return name_;
    }

    Nonogram::Solution Nonogram::solution() const
    {
      return solution_;
    }
  }
}
