#pragma once

#include <nonogram/data/clues/Data.hpp>
#include <nonogram/data/clues/FullIndex.hpp>
#include <nonogram/data/clues/FullIndices.hpp>
#include <nonogram/data/Clues.hpp>
#include <nonogram/data/Solution.hpp>

#include <QtCore/QDataStream>

namespace nonogram::data
{
  using ClueState = bool;

  class ClueStates
  {
  public:
    using Data = clues::Data<ClueState>;

    ClueStates (Solution const&, Clues::Type);

    bool isCrossed (clues::FullIndex) const;
    void cross (clues::FullIndex, ClueState);

    clues::FullIndices toLock() const;
    clues::FullIndices locked() const;
    bool isLocked (clues::FullIndex) const;
    void lock (clues::FullIndices, bool);
    void fillLocks (bool);

    bool canLock() const;
    bool canUnlock() const;

    void reset();

    // serialization
    ClueStates (QDataStream&);
    friend QDataStream& operator<< (QDataStream&, ClueStates const&);

  private:
    Data fromSolution (Solution const&, Clues::Type) const;

    Data data_;
    Data locks_;
  };
}
