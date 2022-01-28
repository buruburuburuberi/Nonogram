#pragma once

#include <nonogram/data/Nonogram.hpp>

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QPaintEvent>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QOpenGLWidget>

#include <optional>

namespace nonogram
{
  namespace gui
  {
    class PlayField : public QOpenGLWidget
    {
      Q_OBJECT

    public:
      PlayField (QWidget* parent);

      void setNonogram (data::Nonogram);

    protected:
      void paintEvent (QPaintEvent*) override;

    private:
      QRect slotRect (int row, int column) const;
      void drawSlot ( QPainter* painter
                    , int row
                    , int column
                    , data::Nonogram::Datum datum
                    );

      int slot_size_;
      int fill_size_;
      std::optional<data::Nonogram> nonogram_;
    };
  }
}
