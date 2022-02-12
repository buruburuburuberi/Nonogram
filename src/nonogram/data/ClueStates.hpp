#pragma once

#include <nonogram/data/VectorOfVectors.hpp>
#include <nonogram/data/Clues.hpp>
#include <nonogram/data/Solution.hpp>

#include <set>

namespace nonogram
{
  namespace file
  {
    class Puzzles;
  }

  namespace data
  {
    using ClueState = bool;

    class ClueStates
    {
    public:
      using Data = VectorOfVectors<ClueState>;

      ClueStates (Solution const&, Clues::Type);
      ClueStates (Data data, Data locks);

      bool isCrossed (FullIndex) const;
      void cross (FullIndex, ClueState);
      Indices toLock() const;
      Indices locked() const;
      bool isLocked (FullIndex) const;
      void lock (Indices, bool);
      void fillLocks (bool);

      bool canLock() const;
      bool canUnlock() const;

      void reset();

    private:
      Data data_;
      Data locks_;

      friend class file::Puzzles;
    };
  }
}
