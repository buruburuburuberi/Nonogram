#pragma once

#include <nonogram/data/Nonogram.hpp>

#include <QtGui/QMouseEvent>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QPaintEvent>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QOpenGLWidget>
#include <QtWidgets/QUndoStack>

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
      PlayField (QUndoStack&, data::Nonogram);

      void setFillMode (data::Answer::Datum);
      void setNonogram (data::Nonogram);
      void checkAnswer();
      void resetAnswer();

      bool canLock() const;
      bool canUnlock() const;
      void lock();
      void unlock();

      void redo();
      void undo();

    signals:
      void solved();

    protected:
      void mouseMoveEvent (QMouseEvent*) override;
      void mousePressEvent (QMouseEvent*) override;
      void mouseReleaseEvent (QMouseEvent*) override;
      void paintGL() override;
      void resizeGL (int width, int height) override;

    private:
      void reset();
      data::Slot fromPosition (QRect, QPoint) const;
      void checkSlot (data::Slot);
      bool fillSlot (QPoint, bool first_hit);
      bool crossClue (QPoint, bool first_hit);
      void finishPuzzle();

      QPoint clueCenter (QRect clues_rect, data::Slot) const;
      void drawClue ( QPainter& painter
                    , data::Solution::ClueType type
                    , data::Slot
                    , bool mark_as_error
                    );

      QPoint slotCenter (data::Slot) const;
      void drawSlot ( QPainter& painter
                    , data::Slot
                    , data::Answer::Datum
                    );

      void drawClues (QPainter& painter, data::Solution::ClueType);
      void drawPuzzle (QPainter& painter);

      void updateRects (QSize window_size);

      QUndoStack& undo_stack_;
      data::Nonogram nonogram_;
      data::Answer::Datum fill_mode_;
      std::size_t font_size_;
      std::size_t slot_size_;
      QRect puzzle_rect_;
      std::map<data::Solution::ClueType, QRect> clues_rects_;
      QRect field_rect_;

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
      std::optional<data::Slot> current_error_slot_;
      bool solved_;
    };
  }
}
