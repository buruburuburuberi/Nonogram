#include <nonogram/data/ClueStates.hpp>

#include <string>

namespace nonogram
{
  namespace data
  {
    ClueStates::ClueStates (Solution const& solution, Clues::Type type)
    : data_ (solution.clueMainSize (type))
    , locks_ (solution.clueMainSize (type))
    {
      for ( MainIndex main_index {0}
          ; main_index.value < solution.clueMainSize (type).value
          ; ++main_index.value
          )
      {
        data_.resize (main_index, solution.clueMinorSize (type, main_index), false);
        locks_.resize (main_index, solution.clueMinorSize (type, main_index), false);
      }
    }

    ClueStates::ClueStates (Data data, Data locks)
    : data_ (std::move (data))
    , locks_ (std::move (locks))
    {}

    bool ClueStates::isCrossed (FullIndex full_index) const
    {
      return data_.at (full_index);
    }

    void ClueStates::cross (FullIndex full_index, bool state)
    {
      if (locks_.at (full_index))
      {
        throw std::logic_error ("Tried to change locked clue.");
      }

      data_.set (full_index, state);
    }

    Indices ClueStates::toLock() const
    {
      return data_.indices_if
          ( [&] (FullIndex index, bool state)
            { return state && !locks_.at (index); }
          );
    }

    Indices ClueStates::locked() const
    {
      return locks_.indices_if ([] (FullIndex, bool state) { return state; });
    }

    bool ClueStates::isLocked (FullIndex full_index) const
    {
      return locks_.at (full_index);
    }

    void ClueStates::lock (Indices indices, ClueState state)
    {
      for (auto const& index : indices)
      {
        locks_.set (index, state);
      }
    }

    void ClueStates::fillLocks (bool state)
    {
      locks_.fill (state);
    }

    bool ClueStates::canLock() const
    {
      return !toLock().empty();
    }

    bool ClueStates::canUnlock() const
    {
      for ( MainIndex main_index {0}
          ; main_index.value < locks_.mainSize().value
          ; ++main_index.value
          )
      {
        for ( MinorIndex minor_index {0}
            ; minor_index.value < locks_.minorSize (main_index).value
            ; ++minor_index.value
            )
        {
          if (locks_.at (main_index, minor_index))
          {
            return true;
          }
        }
      }

      return false;
    }

    void ClueStates::reset()
    {
      data_.fill (false);
      locks_.fill (false);
    }
  }
}
