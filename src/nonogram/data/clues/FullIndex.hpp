#pragma once

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
      struct FullIndex
      {
        MainIndex main;
        MinorIndex minor;

        bool operator== (FullIndex const& rhs) const;
        bool operator< (FullIndex const& rhs) const;
      };
    }
  }
}
