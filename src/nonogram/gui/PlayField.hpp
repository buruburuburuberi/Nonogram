#pragma once

#include <QtGui/QOpenGLFunctions>
#include <QtWidgets/QOpenGLWidget>

namespace nonogram
{
  namespace gui
  {
    class PlayField : public QOpenGLWidget
    {
      Q_OBJECT

    public:
      PlayField (QWidget* parent);

    protected:
      void paintEvent (QPaintEvent *event) override;

    private:
    };
  }
}
