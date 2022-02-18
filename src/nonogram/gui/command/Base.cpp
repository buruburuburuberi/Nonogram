#include <nonogram/gui/command/Base.hpp>

namespace nonogram
{
  namespace gui
  {
    namespace command
    {
      Base::Base (int id, data::Nonogram& nonogram)
      : id_ (id)
      , nonogram_ (nonogram)
      {}

      int Base::id() const
      {
        return id_;
      }
    }
  }
}
