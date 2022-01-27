#pragma once

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

namespace nonogram
{
  namespace data
  {
    class Nonogram
    {
      enum class Type
      { Empty = 0
      , Filled = 1
      , Cross = 2
      , Marked = 3
      };

      using Solution = std::vector<bool>;
      using Current = std::vector<Type>;

      public:
        Nonogram (std::string name, Solution data);

        std::string name() const;
        Solution solution() const;

      private:
        std::string name_;
        Solution solution_;
        Current current_;
    };
  }
}
