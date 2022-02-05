#include <nonogram/data/ClueStates.hpp>

#include <string>

namespace nonogram
{
  namespace data
  {
    ClueStates::ClueStates (Solution const& solution, Clues::Type type)
    : type_ (type)
    , data_ (solution.clueMainSize (type))
    , locks_ (solution.clueMainSize (type))
    , to_lock_()
    {
      for ( MainIndex main_index {0}
          ; main_index.value < solution.clueMainSize (type_).value
          ; ++main_index.value
          )
      {
        data_.resize (main_index, solution.clueMinorSize (type, main_index), false);
        locks_.resize (main_index, solution.clueMinorSize (type, main_index), false);
      }
    }

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

      if (state)
      {
        to_lock_.insert (full_index);
      }
      else
      {
        to_lock_.erase (full_index);
      }
    }

    Indices ClueStates::toLock() const
    {
      return to_lock_;
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

      if (state)
      {
        to_lock_.clear();
      }
      else
      {
        to_lock_ = data_.indices_if ([] (FullIndex, bool state) { return state; });
      }
    }

    bool ClueStates::canLock() const
    {
      return !to_lock_.empty();
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
      to_lock_.clear();
    }
  }
}
