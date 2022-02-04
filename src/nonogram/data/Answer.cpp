#include <nonogram/data/Answer.hpp>

#include <string>

namespace nonogram
{
  namespace data
  {
    Answer::Answer (Solution const& solution)
    : data_ (solution.dataColumns(), solution.dataRows(), Datum::Empty)
    , data_locks_ (solution.dataColumns(), solution.dataRows(), false)
    , data_to_lock_()
    {
      for (auto const& type : Solution::all_clue_types)
      {
        clues_.emplace
          ( std::piecewise_construct
          , std::forward_as_tuple (type)
          , std::forward_as_tuple (solution, type)
          );
      }
    }

    Answer::Datum Answer::at (Slot slot) const
    {
      return data_.at (slot);
    }

    void Answer::fill (Slot slot, Datum datum)
    {
      if (data_locks_.at (slot))
      {
        throw std::logic_error ("Tried to change locked datum.");
      }

      data_.set (slot, datum);

      if (datum != Datum::Empty)
      {
        data_to_lock_.insert (slot);
      }
      else
      {
        data_to_lock_.erase (slot);
      }
    }

    Slots Answer::dataToLock() const
    {
      return data_to_lock_;
    }

    Slots Answer::lockedData() const
    {
      return data_locks_.slots_if ([] (Slot, bool state) { return state; });
    }

    bool Answer::isDatumLocked (Slot slot) const
    {
      return data_locks_.at (slot);
    }

    void Answer::lockData (Slots slots, bool state)
    {
      for (auto const& slot : slots)
      {
        data_locks_.set (slot, state);
      }

      if (state)
      {
        data_to_lock_.clear();
      }
      else
      {
        data_to_lock_ =
          data_.slots_if
            ([] (Slot, Datum datum) { return datum != Datum::Empty; });
      }
    }

    bool Answer::isCrossed (Solution::ClueType type, Slot slot) const
    {
      return clues_.at (type).isCrossed (slot);
    }

    void Answer::cross (Solution::ClueType type, Slot slot, bool state)
    {
      clues_.at (type).cross (slot, state);
    }

    Solution::ClueSlots Answer::cluesToLock() const
    {
      Solution::ClueSlots clue_slots;

      for (auto const& type : Solution::all_clue_types)
      {
        clue_slots.emplace (type, clues_.at (type).toLock());
      }

      return clue_slots;
    }

    Solution::ClueSlots Answer::lockedClues() const
    {
      Solution::ClueSlots clue_slots;

      for (auto const& type : Solution::all_clue_types)
      {
        clue_slots.emplace (type, clues_.at (type).locked());
      }

      return clue_slots;
    }

    bool Answer::isClueLocked (Solution::ClueType type, Slot slot) const
    {
      return clues_.at (type).isLocked (slot);
    }

    void Answer::lockClues (Solution::ClueSlots clue_slots, bool state)
    {
      for (auto const& type : Solution::all_clue_types)
      {
        clues_.at (type).lock (clue_slots.at (type), state);
      }
    }

    bool Answer::canLock() const
    {
      auto hasCluesToLock
        ( [&]
          {
            for (auto const& type : Solution::all_clue_types)
            {
              if (clues_.at (type).canLock())
              {
                return true;
              }
            }

            return false;
          }
        );

      return !data_to_lock_.empty() || hasCluesToLock();
    }

    bool Answer::canUnlock() const
    {
      for (Row row {0}; row.value < data_locks_.rows().value; ++row.value)
      {
        for ( Column column {0}
            ; column.value < data_locks_.columns().value
            ; ++column.value
            )
        {
          if (data_locks_.at ({column, row}))
          {
            return true;
          }
        }
      }

      for (auto const& type : Solution::all_clue_types)
      {
        if (clues_.at (type).canUnlock())
        {
          return true;
        }
      }

      return false;
    }

    void Answer::reset()
    {
      data_.fill (Answer::Datum::Empty);
      data_locks_.fill (false);
      data_to_lock_.clear();

      for (auto const& type : Solution::all_clue_types)
      {
        clues_.at (type).reset();
      }
    }
  }
}
