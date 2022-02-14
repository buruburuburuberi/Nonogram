#include <nonogram/util/ScopedQPainterState.hpp>

namespace nonogram
{
  namespace util
  {
    ScopedQPainterState::ScopedQPainterState (QPainter& painter)
    : painter_ (painter)
    {
      painter_.save();
    }

    ScopedQPainterState::~ScopedQPainterState()
    {
      painter_.restore();
    }
  }
}
