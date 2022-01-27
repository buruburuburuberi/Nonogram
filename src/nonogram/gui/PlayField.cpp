#include <nonogram/gui/PlayField.hpp>

#include <QtGui/QPainter>

namespace nonogram
{
  namespace gui
  {
    PlayField::PlayField (QWidget *parent)
    : QOpenGLWidget (parent)
    {
      setAutoFillBackground (true);
    }

    void PlayField::paintEvent (QPaintEvent *event)
    {
      QPainter painter (this);
      painter.fillRect (rect(), Qt::black);
    }
  };
}
