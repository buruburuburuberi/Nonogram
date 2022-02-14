#include <nonogram/data/Answer.hpp>

#include <string>

namespace nonogram
{
  namespace data
  {
    Answer::Answer (Solution const& solution)
    : data_ (solution.dataColumns(), solution.dataRows(), Datum::Empty)
    , data_locks_ (solution.dataColumns(), solution.dataRows(), false)
    {
      for (auto const& type : Clues::all_types)
      {
        clue_states_.emplace
          ( std::piecewise_construct
          , std::forward_as_tuple (type)
          , std::forward_as_tuple (solution, type)
          );
      }
    }

    Answer::Answer (Data data, DataLocks data_locks, CluesStates clue_states)
    : data_ (std::move (data))
    , data_locks_ (std::move (data_locks))
    , clue_states_ (std::move (clue_states))
    {}

    Answer::Datum Answer::at (Slot slot) const
    {
      return data_.at (slot);
    }

    void Answer::fillData (Slot slot, Datum datum)
    {
      if (data_locks_.at (slot))
      {
        throw std::logic_error ("Tried to change locked datum.");
      }

      data_.set (slot, datum);
    }

    Slots Answer::dataToLock() const
    {
      return data_.slots_if
          ( [&] (Slot slot, Datum datum)
            { return datum != Datum::Empty && !data_locks_.at (slot); }
          );
    }

    Slots Answer::lockedData() const
    {
      return data_locks_.slots_if ([] (Slot, bool state) { return state; });
    }

    bool Answer::isDatumLocked (Slot slot) const
    {
      return data_locks_.at (slot);
    }

    void Answer::lockData (Slots to_lock, bool state)
    {
      for (auto const& slot : to_lock)
      {
        data_locks_.set (slot, state);
      }
    }

    void Answer::fillDataLocks (bool state)
    {
      data_locks_.fill (state);
    }

    bool Answer::isCrossed (Clues::Type type, FullIndex full_index) const
    {
      return clue_states_.at (type).isCrossed (full_index);
    }

    void Answer::cross (Clues::Type type, FullIndex full_index, bool state)
    {
      clue_states_.at (type).cross (full_index, state);
    }

    Solution::ClueIndices Answer::cluesToLock() const
    {
      Solution::ClueIndices indices;

      for (auto const& type : Clues::all_types)
      {
        indices.emplace (type, clue_states_.at (type).toLock());
      }

      return indices;
    }

    Solution::ClueIndices Answer::lockedClues() const
    {
      Solution::ClueIndices indices;

      for (auto const& type : Clues::all_types)
      {
        indices.emplace (type, clue_states_.at (type).locked());
      }

      return indices;
    }

    bool Answer::isClueLocked (Clues::Type type, FullIndex full_index) const
    {
      return clue_states_.at (type).isLocked (full_index);
    }

    void Answer::lockClues (Solution::ClueIndices indices, bool state)
    {
      for (auto const& type : Clues::all_types)
      {
        clue_states_.at (type).lock (indices.at (type), state);
      }
    }

    void Answer::fillClueLocks (bool state)
    {
      for (auto const& type : Clues::all_types)
      {
        clue_states_.at (type).fillLocks (state);
      }
    }

    bool Answer::canLock() const
    {
      auto hasCluesToLock
        ( [&]
          {
            for (auto const& type : Clues::all_types)
            {
              if (clue_states_.at (type).canLock())
              {
                return true;
              }
            }

            return false;
          }
        );

      return !dataToLock().empty() || hasCluesToLock();
    }

    bool Answer::canUnlock() const
    {
      for (Row row {0}; row < data_locks_.rows(); ++row)
      {
        for (Column column {0}; column < data_locks_.columns(); ++column)
        {
          if (data_locks_.at ({column, row}))
          {
            return true;
          }
        }
      }

      for (auto const& type : Clues::all_types)
      {
        if (clue_states_.at (type).canUnlock())
        {
          return true;
        }
      }

      return false;
    }

    bool Answer::isEmpty() const
    {
      return !canLock() && !canUnlock();
    }

    void Answer::reset()
    {
      data_.fill (Answer::Datum::Empty);
      data_locks_.fill (false);

      for (auto const& type : Clues::all_types)
      {
        clue_states_.at (type).reset();
      }
    }

    QDataStream& operator>> (QDataStream& ds, Answer::Datum& datum)
    {
      unsigned int value;
      ds >> value;
      datum = static_cast<Answer::Datum> (value);

      return ds;
    }
    QDataStream& operator<< (QDataStream& ds, Answer::Datum const& datum)
    {
      ds << static_cast<unsigned int> (datum);

      return ds;
    }

    Answer::Answer (QDataStream& ds)
    : data_ (ds)
    , data_locks_ (ds)
    {
      for (auto const& type : Clues::all_types)
      {
        clue_states_.emplace
          ( std::piecewise_construct
          , std::forward_as_tuple (type)
          , std::forward_as_tuple (ds)
          );
      }
    }
    QDataStream& operator<< (QDataStream& ds, Answer const& answer)
    {
      ds << answer.data_;
      ds << answer.data_locks_;

      for (auto const& type : Clues::all_types)
      {
        ds << answer.clue_states_.at (type);
      }

      return ds;
    }
  }
}
