#include <nonogram/data/ClueStates.hpp>

#include <string>

namespace nonogram
{
  namespace data
  {
    ClueStates::ClueStates (Solution const& solution, Solution::ClueType type)
    : type_ (type)
    , data_ (solution.clueColumns (type), solution.clueRows (type), false)
    , number_of_elements_
        ( type == Solution::ClueType::Left || type == Solution::ClueType::Right
        ? data_.rows().value
        : data_.columns().value
        )
    , locks_ (solution.clueColumns (type), solution.clueRows (type), false)
    , to_lock_()
    {
      for (Row row {0}; row.value < data_.rows().value; ++row.value)
      {
        for ( Column column {0}
            ; column.value < data_.columns().value
            ; ++column.value
            )
        {

        }
      }
    }

    bool ClueStates::isCrossed (Slot slot) const
    {
      return data_.at (slot);
    }

    void ClueStates::cross (Slot slot, bool state)
    {
      if (locks_.at (slot))
      {
        throw std::logic_error ("Tried to change locked clue.");
      }

      data_.set (slot, state);

      if (state)
      {
        to_lock_.insert (slot);
      }
      else
      {
        to_lock_.erase (slot);
      }
    }

    Slots ClueStates::toLock() const
    {
      return to_lock_;
    }

    Slots ClueStates::locked() const
    {
      return locks_.slots_if ([] (Slot, bool state) { return state; });
    }

    bool ClueStates::isLocked (Slot slot) const
    {
      return locks_.at (slot);
    }

    void ClueStates::lock (Slots slots, ClueState state)
    {
      for (auto const& slot : slots)
      {
        locks_.set (slot, state);
      }

      if (state)
      {
        to_lock_.clear();
      }
      else
      {
        to_lock_ = data_.slots_if ([] (Slot, bool state) { return state; });
      }
    }

    bool ClueStates::canLock() const
    {
      return !to_lock_.empty();
    }

    bool ClueStates::canUnlock() const
    {
      for (Row row {0}; row.value < locks_.rows().value; ++row.value)
      {
        for ( Column column {0}
            ; column.value < locks_.columns().value
            ; ++column.value
            )
        {
          if (locks_.at ({column, row}))
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
