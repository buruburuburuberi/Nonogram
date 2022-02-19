#include <nonogram/gui/command/Lock.hpp>

namespace nonogram::gui::command
{
  Lock::Lock (data::Nonogram& nonogram, bool unlock)
  : Base (-1, nonogram)
  , unlock_ (unlock)
  , data_cells_ (unlock_ ? nonogram_.lockedData() : nonogram_.dataToLock())
  , clue_indices_ (unlock_ ? nonogram_.lockedClues() : nonogram_.cluesToLock())
  {
    setText (unlock_ ? "Unlock" : "Lock");
  }

  Lock* Lock::lock (data::Nonogram& nonogram)
  {
    return new Lock (nonogram, false);
  }

  Lock* Lock::unlock (data::Nonogram& nonogram)
  {
    return new Lock (nonogram, true);
  }

  void Lock::undo()
  {
    nonogram_.lockData (data_cells_, unlock_);
    nonogram_.lockClues (clue_indices_, unlock_);
  }

  void Lock::redo()
  {
    nonogram_.lockData (data_cells_, !unlock_);
    nonogram_.lockClues (clue_indices_, !unlock_);
  }
}
