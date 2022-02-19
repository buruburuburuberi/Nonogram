#pragma once

#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QtCore/QSize>

namespace nonogram::util
{
  class CenteredRect
  {
  public:
    CenteredRect (int x, int y, int width, int height);
    CenteredRect (QPoint const, QSize const);

    operator QRect() const;

  private:
    QRect rect_;
  };
}
