#include <nonogram/data/Nonogram.hpp>

#include <nonogram/file/Puzzles.hpp>

#include <QtCore/QStringList>

#include <cstddef>
#include <string>
#include <tuple>
#include <vector>

namespace nonogram
{
  namespace data
  {
    Nonogram::Pack Nonogram::internalPack()
    {
      return Pack ("Internal");
    }

    Nonogram::ID::ID (Pack _pack, Puzzle _puzzle)
    : pack (_pack)
    , puzzle (_puzzle)
    {}

    Nonogram::ID::ID (QString string)
    {
      auto const strings (string.split ("/"));
      pack = Pack (strings.at (0));
      puzzle = Puzzle (strings.at (1));
    }

    bool Nonogram::ID::internal() const
    {
      return pack == internalPack();
    }

    QString Nonogram::ID::toString() const
    {
      return QString ("%1/%2").arg (pack.name).arg (puzzle.name);
    }

    bool Nonogram::ID::operator== (ID const& rhs) const
    {
      return std::tie (pack, puzzle) == std::tie (rhs.pack, rhs.puzzle);
    }
    bool Nonogram::ID::operator< (ID const& rhs) const
    {
      return std::tie (pack, puzzle) < std::tie (rhs.pack, rhs.puzzle);
    }

    Nonogram::Nonogram ( data::Nonogram::ID id
                       , Solution solution
                       , Answer answer
                       )
    : id_ (std::move (id))
    , solution_ (std::move (solution))
    , answer_ (std::move (answer))
    {}

    Nonogram::Nonogram (data::Nonogram::ID id, Solution solution)
    : Nonogram (std::move (id), solution, Answer (solution))
    {}

    Nonogram::ID Nonogram::id() const
    {
      return id_;
    }

    grid::Row Nonogram::dataRows() const
    {
      return solution_.dataRows();
    }

    grid::Column Nonogram::dataColumns() const
    {
      return solution_.dataColumns();
    }

    Solution::State Nonogram::solution (grid::Cell cell) const
    {
      return solution_.at (cell);
    }

    Answer::Datum Nonogram::answer (grid::Cell cell) const
    {
      return answer_.at (cell);
    }

    void Nonogram::fillData (grid::Cell cell, Answer::Datum datum)
    {
      answer_.fillData (cell, datum);
    }

    grid::Cells Nonogram::dataToLock() const
    {
      return answer_.dataToLock();
    }

    grid::Cells Nonogram::lockedData() const
    {
      return answer_.lockedData();
    }

    bool Nonogram::isDatumLocked (grid::Cell cell) const
    {
      return answer_.isDatumLocked (cell);
    }

    void Nonogram::lockData (grid::Cells to_move, bool state)
    {
      answer_.lockData (std::move (to_move), state);
    }

    void Nonogram::fillDataLocks (bool state)
    {
      answer_.fillDataLocks (state);
    }

    clues::MinorIndex Nonogram::maxNumberOfClues (Clues::Type type) const
    {
      return solution_.maxNumberOfClues (type);
    }

    clues::MainIndex Nonogram::clueMainSize (Clues::Type type) const
    {
      return solution_.clueMainSize (type);
    }

    clues::MinorIndex Nonogram::clueMinorSize ( Clues::Type type
                                              , clues::MainIndex main_index
                                              ) const
    {
      return solution_.clueMinorSize (type, main_index);
    }

    Clues::Value Nonogram::clue ( Clues::Type type
                                , clues::FullIndex full_index
                                ) const
    {
      return solution_.clue (type, full_index);
    }

    ClueState Nonogram::isCrossed (Clues::Type type, clues::FullIndex full_index) const
    {
      return answer_.isCrossed (type, full_index);
    }

    void Nonogram::cross ( Clues::Type type
                         , clues::FullIndex full_index
                         , ClueState state
                         )
    {
      answer_.cross (type, full_index, state);
    }

    Solution::ClueIndices Nonogram::cluesToLock() const
    {
      return answer_.cluesToLock();
    }

    Solution::ClueIndices Nonogram::lockedClues() const
    {
      return answer_.lockedClues();
    }

    bool Nonogram::isClueLocked ( Clues::Type type
                                , clues::FullIndex full_index
                                ) const
    {
      return answer_.isClueLocked (type, full_index);
    }

    void Nonogram::lockClues (Solution::ClueIndices indices, bool state)
    {
      answer_.lockClues (std::move (indices), state);
    }

    void Nonogram::fillClueLocks (bool state)
    {
      answer_.fillClueLocks (state);
    }

    bool Nonogram::canLock() const
    {
      return answer_.canLock();
    }

    bool Nonogram::canUnlock() const
    {
      return answer_.canUnlock();
    }

    bool Nonogram::isEmpty() const
    {
      return answer_.isEmpty();
    }

    bool Nonogram::isMistake (grid::Cell cell) const
    {
      auto const datum (answer_.at (cell));
      return ( (datum == Answer::Datum::Filled)
            || (datum == Answer::Datum::Crossed)
             )
          && solution_.at (cell) != (datum == data::Answer::Datum::Filled);
    }

    std::optional<grid::Cell> Nonogram::findFirstMistake() const
    {
      for (grid::Row row {0}; row < dataRows(); ++row)
      {
        for (grid::Column column {0}; column < dataColumns(); ++column)
        {
          grid::Cell const cell {column, row};
          if (isMistake (cell))
          {
            return cell;
          }
        }
      }

      return std::nullopt;
    }

    bool Nonogram::isSolved() const
    {
      for (grid::Row row {0}; row < dataRows(); ++row)
      {
        for (grid::Column column {0}; column < dataColumns(); ++column)
        {
          grid::Cell const cell {column, row};
          auto const datum (answer_.at (cell));
          if (solution_.at (cell) != (datum == data::Answer::Datum::Filled))
          {
            return false;
          }
        }
      }

      return true;
    }

    void Nonogram::resetAnswer()
    {
      answer_.reset();
    }

    Answer const& Nonogram::answer() const
    {
      return answer_;
    }
  }
}
