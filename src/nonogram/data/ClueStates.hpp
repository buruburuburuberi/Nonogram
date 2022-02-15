#pragma once

#include <nonogram/data/VectorOfVectors.hpp>
#include <nonogram/data/Clues.hpp>
#include <nonogram/data/Solution.hpp>

#include <QtCore/QDataStream>

namespace nonogram
{
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
      FullIndices toLock() const;
      FullIndices locked() const;
      bool isLocked (FullIndex) const;
      void lock (FullIndices, bool);
      void fillLocks (bool);

      bool canLock() const;
      bool canUnlock() const;

      void reset();

      // serialization
      ClueStates (QDataStream&);
      friend QDataStream& operator<< (QDataStream&, ClueStates const&);

    private:
      Data data_;
      Data locks_;
    };
  }
}
