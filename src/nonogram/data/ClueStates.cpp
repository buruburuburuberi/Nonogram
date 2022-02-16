#include <nonogram/data/ClueStates.hpp>

#include <nonogram/data/clues/MainIndex.hpp>
#include <nonogram/data/clues/MinorIndex.hpp>

#include <string>

namespace nonogram
{
  namespace data
  {
    ClueStates::ClueStates (Solution const& solution, Clues::Type type)
    : data_ (fromSolution (solution, type))
    , locks_ (data_)
    {}

    ClueStates::Data ClueStates::fromSolution ( Solution const& solution
                                              , Clues::Type type
                                              ) const
    {
      auto const main_size (solution.clueMainSize (type));
      Data::Container data (main_size.value);

      for ( clues::MainIndex main_index {0}
          ; main_index < main_size
          ; ++main_index
          )
      {
        data.at (main_index.value).resize
          (solution.clueMinorSize (type, main_index).value, false);
      }

      return {data};
    }

    bool ClueStates::isCrossed (clues::FullIndex full_index) const
    {
      return data_.at (full_index);
    }

    void ClueStates::cross (clues::FullIndex full_index, bool state)
    {
      if (locks_.at (full_index))
      {
        throw std::logic_error ("Tried to change locked clue.");
      }

      data_.set (full_index, state);
    }

    clues::FullIndices ClueStates::toLock() const
    {
      return data_.indices_if
          ( [&] (clues::FullIndex index, bool state)
            { return state && !locks_.at (index); }
          );
    }

    clues::FullIndices ClueStates::locked() const
    {
      return locks_.indices_if
          ([] (clues::FullIndex, bool state) { return state; });
    }

    bool ClueStates::isLocked (clues::FullIndex full_index) const
    {
      return locks_.at (full_index);
    }

    void ClueStates::lock (clues::FullIndices indices, ClueState state)
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
      for (auto const& lock : locks_)
      {
        if (lock.second)
        {
          return true;
        }
      }

      return false;
    }

    void ClueStates::reset()
    {
      data_.fill (false);
      locks_.fill (false);
    }

    ClueStates::ClueStates (QDataStream& ds)
     : data_ (ds)
     , locks_ (ds)
     {}

     QDataStream& operator<< (QDataStream& ds, ClueStates const& clue_states)
     {
       ds << clue_states.data_;
       ds << clue_states.locks_;

       return ds;
     }
  }
}
