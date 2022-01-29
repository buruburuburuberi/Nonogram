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
      void paintGL() override;
      void resizeGL (int width, int height) override;

    private:
      QPoint clueCenter (QRect clues_rect, data::Column, data::Row) const;
      void drawClue ( QPainter& painter
                    , QRect clues_rect
                    , data::Column
                    , data::Row
                    , std::optional<data::Solution::Clue>
                    );

      QPoint slotCenter (data::Column, data::Row) const;
      void drawSlot ( QPainter& painter
                    , data::Column
                    , data::Row
                    , data::Nonogram::Datum
                    );

      void drawClues (QPainter& painter);
      void drawPuzzle (QPainter& painter);

      struct NonogramData
      {
        NonogramData (data::Nonogram, QSize window_size);

        void update (QSize window_size);

        int slot_size;
        data::Nonogram data;
        QRect puzzle_rect;
        QRect left_clues_rect;
        QRect right_clues_rect;
        QRect top_clues_rect;
        QRect bottom_clues_rect;
        QRect field_rect;
      };

      std::optional<NonogramData> nonogram_;
    };
  }
}
