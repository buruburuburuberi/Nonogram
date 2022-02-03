#include <nonogram/gui/command/Lock.hpp>

namespace nonogram
{
  namespace gui
  {
    namespace command
    {
      Lock::Lock (data::Nonogram& nonogram, bool unlock)
      : Base (-1, nonogram)
      , unlock_ (unlock)
      , data_slots_ (unlock_ ? nonogram_.lockedData() : nonogram_.dataToLock())
      , clue_slots_ (unlock_ ? nonogram_.lockedClues() : nonogram_.cluesToLock())
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
        nonogram_.lockData (data_slots_, unlock_);
        nonogram_.lockClues (clue_slots_, unlock_);
      }

      void Lock::redo()
      {
        nonogram_.lockData (data_slots_, !unlock_);
        nonogram_.lockClues (clue_slots_, !unlock_);
      }
    }
  }
}
