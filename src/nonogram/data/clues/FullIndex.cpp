#include <nonogram/data/clues/FullIndex.hpp>

#include <tuple>

namespace nonogram::data::clues
{
  bool FullIndex::operator== (FullIndex const& rhs) const
  {
    return std::tie (main, minor) == std::tie (rhs.main, rhs.minor);
  }

  bool FullIndex::operator< (FullIndex const& rhs) const
  {
    return std::tie (main, minor) < std::tie (rhs.main, rhs.minor);
  }
}
