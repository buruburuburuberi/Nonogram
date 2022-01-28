#pragma once

#include <nonogram/data/Array2D.hpp>

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

namespace nonogram
{
  namespace data
  {
    class Nonogram
    {
    public:
      enum class Datum
      { Empty = 0
      , Filled = 1
      , Cross = 2
      , Marked = 3
      };

      using Solution = Array2D<bool>;
      using Answer = Array2D<Datum>;

      Nonogram (std::string name, Solution data);

      std::string name() const;
      unsigned int rows() const;
      unsigned int columns() const;
      Datum at (unsigned int column, unsigned int row) const;

    private:
      std::string name_;
      Solution solution_;
      Answer answer_;
    };
  }
}
