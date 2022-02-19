#pragma once

#include <nonogram/data/clues/MainIndex.hpp>
#include <nonogram/data/clues/MinorIndex.hpp>

namespace nonogram::data::clues
{
  struct FullIndex
  {
    MainIndex main;
    MinorIndex minor;

    bool operator== (FullIndex const& rhs) const;
    bool operator< (FullIndex const& rhs) const;
  };
}
