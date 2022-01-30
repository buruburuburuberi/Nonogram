#pragma once

#include <nonogram/data/Nonogram.hpp>

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QPaintEvent>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QOpenGLWidget>

#include <optional>
#include <map>

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
                    , data::Solution::ClueType type
                    , data::Column
                    , data::Row
                    );

      QPoint slotCenter (data::Column, data::Row) const;
      void drawSlot ( QPainter& painter
                    , data::Column
                    , data::Row
                    , data::Answer::Datum
                    );

      void drawClues (QPainter& painter, data::Solution::ClueType);
      void drawPuzzle (QPainter& painter);

      struct NonogramData
      {
        NonogramData (data::Nonogram, QSize window_size);

        void update (QSize window_size);

        int font_size;
        int slot_size;
        data::Nonogram data;
        QRect puzzle_rect;
        std::map<data::Solution::ClueType, QRect> clues_rects;
        QRect field_rect;
      };

      std::optional<NonogramData> nonogram_;
    };
  }
}
