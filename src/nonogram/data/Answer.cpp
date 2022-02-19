#include <nonogram/data/Answer.hpp>

#include <algorithm>
#include <stdexcept>
#include <tuple>
#include <utility>

namespace nonogram::data
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

  Answer::Data const& Answer::data() const
  {
    return data_;
  }

  Answer::Datum Answer::datum (grid::Cell cell) const
  {
    return data_.at (cell);
  }

  void Answer::fillData (grid::Cell cell, Datum datum)
  {
    if (data_locks_.at (cell))
    {
      throw std::logic_error ("Tried to change locked datum.");
    }

    data_.set (cell, datum);
  }

  grid::Cells Answer::dataToLock() const
  {
    return data_.cells_if
        ( [&] (grid::Cell cell, Datum datum)
          { return datum != Datum::Empty && !data_locks_.at (cell); }
        );
  }

  grid::Cells Answer::lockedData() const
  {
    return data_locks_.cells_if ([] (grid::Cell, bool state) { return state; });
  }

  bool Answer::isDatumLocked (grid::Cell cell) const
  {
    return data_locks_.at (cell);
  }

  void Answer::lockData (grid::Cells to_lock, bool state)
  {
    for (auto const& cell : to_lock)
    {
      data_locks_.set (cell, state);
    }
  }

  void Answer::fillDataLocks (bool state)
  {
    data_locks_.fill (state);
  }

  bool Answer::isCrossed (Clues::Type type, clues::FullIndex full_index) const
  {
    return clue_states_.at (type).isCrossed (full_index);
  }

  void Answer::cross (Clues::Type type, clues::FullIndex full_index, bool state)
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

  bool Answer::isClueLocked (Clues::Type type, clues::FullIndex full_index) const
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
    for (auto& [type, states] : clue_states_)
    {
      states.fillLocks (state);
    }
  }

  bool Answer::canLock() const
  {
    return data_.any_of
           ( [&] (grid::Cell cell, Datum datum)
             { return datum != Datum::Empty && !data_locks_.at (cell); }
           )
        || std::any_of
             ( clue_states_.begin()
             , clue_states_.end()
             , [&] (auto const& clue_state)
               { return clue_state.second.canLock(); }
             );
  }

  bool Answer::canUnlock() const
  {
    return data_locks_.any_of ([&] (grid::Cell, bool state) { return state; })
        || std::any_of
             ( clue_states_.begin()
             , clue_states_.end()
             , [&] (auto const& clue_state)
               { return clue_state.second.canUnlock(); }
             );
  }

  bool Answer::isEmpty() const
  {
    return !canLock() && !canUnlock();
  }

  void Answer::reset()
  {
    data_.fill (Answer::Datum::Empty);
    data_locks_.fill (false);

    for (auto& [type, states] : clue_states_)
    {
      states.reset();
    }
  }

  QDataStream& operator>> (QDataStream& ds, Answer::Datum& datum)
  {
    int value;
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

    for (auto const& [type, states] : answer.clue_states_)
    {
      ds << states;
    }

    return ds;
  }
}
