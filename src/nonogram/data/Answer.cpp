#include <nonogram/data/Answer.hpp>

#include <string>

namespace nonogram
{
  namespace data
  {
    Answer::Answer (Solution const& solution)
    : data_ (solution.dataColumns(), solution.dataRows())
    , data_locks_ (solution.dataColumns(), solution.dataRows())
    {
      reset();

      for (auto const& type : Solution::all_clue_types)
      {
        clue_states_.emplace
          ( std::piecewise_construct
          , std::make_tuple (type)
          , std::make_tuple
              ( solution.clueColumns (type)
              , solution.clueRows (type)
              , false
              )
          );

        clue_locks_.emplace
          ( std::piecewise_construct
          , std::make_tuple (type)
          , std::make_tuple
              ( solution.clueColumns (type)
              , solution.clueRows (type)
              , false
              )
          );

        clues_to_lock_[type] = {};
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

    Answer::ClueState Answer::isCrossed ( Solution::ClueType type
                                        , Slot slot
                                        ) const
    {
      return clue_states_.at (type).at (slot);
    }

    void Answer::cross ( Solution::ClueType type
                       , Slot slot
                       , ClueState state
                       )
    {
      if (clue_locks_.at (type).at (slot))
      {
        throw std::logic_error ("Tried to change locked clue.");
      }

      clue_states_.at (type).set (slot, state);

      if (state)
      {
        clues_to_lock_.at (type).insert (slot);
      }
      else
      {
        clues_to_lock_.at (type).erase (slot);
      }
    }

    Solution::ClueSlots Answer::cluesToLock() const
    {
      return clues_to_lock_;
    }

    Solution::ClueSlots Answer::lockedClues() const
    {
      Solution::ClueSlots clue_slots;

      for (auto const& type : Solution::all_clue_types)
      {
        clue_slots.emplace
          ( type
          , clue_locks_.at (type).slots_if
              ([] (Slot, bool state) { return state; })
          );
      }

      return clue_slots;
    }

    bool Answer::isClueLocked (Solution::ClueType type, Slot slot) const
    {
      return clue_locks_.at (type).at (slot);
    }

    void Answer::lockClues (Solution::ClueSlots clue_slots, bool state)
    {
      for (auto const& clue_slot : clue_slots)
      {
        for (auto const& slot : clue_slot.second)
        {
          clue_locks_.at (clue_slot.first).set (slot, state);
        }
      }

      for (auto const& type : Solution::all_clue_types)
      {
        if (state)
        {
          clues_to_lock_.at (type).clear();
        }
        else
        {
          clues_to_lock_.at (type) =
            clue_states_.at (type).slots_if
              ([] (Slot, bool state) { return state; });
        }
      }
    }

    bool Answer::canLock() const
    {
      auto hasCluesToLock
        ( [&]
          {
            for (auto const& type : Solution::all_clue_types)
            {
              if (!clues_to_lock_.at (type).empty())
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
        for ( Row row {0}
            ; row.value < clue_locks_.at (type).rows().value
            ; ++row.value
            )
        {
          for ( Column column {0}
              ; column.value < clue_locks_.at (type).columns().value
              ; ++column.value
              )
          {
            if (clue_locks_.at (type).at ({column, row}))
            {
              return true;
            }
          }
        }
      }

      return false;
    }

    void Answer::reset()
    {
      data_.fill (Answer::Datum::Empty);
      for (auto& clue_states : clue_states_)
      {
        clue_states.second.fill (false);
      }
    }
  }
}
