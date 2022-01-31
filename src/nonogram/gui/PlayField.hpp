#pragma once

#include <nonogram/data/Nonogram.hpp>

#include <QtGui/QMouseEvent>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QPaintEvent>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QOpenGLWidget>

#include <optional>
#include <map>
#include <variant>

namespace nonogram
{
  namespace gui
  {
    class PlayField : public QOpenGLWidget
    {
      Q_OBJECT

    public:
      PlayField (QWidget* parent);

      void setFillMode (data::Answer::Datum);
      void setNonogram (data::Nonogram);
      void resetAnswer();

    protected:
      void mouseMoveEvent (QMouseEvent*) override;
      void mousePressEvent (QMouseEvent*) override;
      void mouseReleaseEvent (QMouseEvent*) override;
      void paintGL() override;
      void resizeGL (int width, int height) override;

    private:
      data::Slot fromPosition (QRect, QPoint) const;
      bool fillSlot (QPoint);
      bool crossClue (QPoint, bool first_press);

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

        std::size_t font_size;
        std::size_t slot_size;
        data::Nonogram data;
        QRect puzzle_rect;
        std::map<data::Solution::ClueType, QRect> clues_rects;
        QRect field_rect;
      };

      std::optional<NonogramData> nonogram_;
      data::Answer::Datum fill_mode_;

      struct ClueHit
      {
        data::Solution::ClueType type;
        data::Slot current_slot;
        data::Answer::ClueState state;
      };

      struct DataHit
      {
        data::Slot current_slot;
        data::Answer::Datum datum;
      };

      using HitType = std::variant<ClueHit, DataHit>;
      std::optional<HitType> current_hit_;

    };
  }
}
