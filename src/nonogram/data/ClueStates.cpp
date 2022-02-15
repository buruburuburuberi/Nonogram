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
      for ( clues::MainIndex main_index {0}
          ; main_index < solution.clueMainSize (type)
          ; ++main_index
          )
      {
        data_.resize ( main_index
                     , solution.clueMinorSize (type, main_index)
                     , false
                     );
        locks_.resize ( main_index
                      , solution.clueMinorSize (type, main_index)
                      , false
                      );
      }
    }

    ClueStates::ClueStates (Data data, Data locks)
    : data_ (std::move (data))
    , locks_ (std::move (locks))
    {}

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
      for ( clues::MainIndex main_index {0}
          ; main_index < locks_.mainSize()
          ; ++main_index
          )
      {
        for ( clues::MinorIndex minor_index {0}
            ; minor_index < locks_.minorSize (main_index)
            ; ++minor_index
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
