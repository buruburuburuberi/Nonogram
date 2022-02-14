#pragma once

#include <QtGui/QPainter>

namespace nonogram
{
  namespace util
  {
    class ScopedQPainterState
    {
    public:
      ScopedQPainterState (QPainter&);
      ~ScopedQPainterState();

    private:
      QPainter& painter_;
    };
  }
}
