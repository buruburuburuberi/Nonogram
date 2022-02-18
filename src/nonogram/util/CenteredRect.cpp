#include <nonogram/util/CenteredRect.hpp>

namespace nonogram
{
  namespace util
  {
    CenteredRect::CenteredRect (int x, int y, int width, int height)
    : rect_ (x - (width - 1)/2, y - (height - 1)/2, width, height)
    {}

    CenteredRect::CenteredRect (QPoint const center, QSize const size)
    : CenteredRect (center.x(), center.y(), size.width(), size.height())
    {}

    CenteredRect::operator QRect() const
    {
      return rect_;
    }
  }
}
