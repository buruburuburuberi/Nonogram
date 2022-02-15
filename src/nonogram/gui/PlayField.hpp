#pragma once

#include <nonogram/data/clues/FullIndex.hpp>
#include <nonogram/data/grid/Cell.hpp>
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
      enum class FieldType
      { LeftClues
      , RightClues
      , TopClues
      , BottomClues
      , Puzzle
      };

      PlayField (QColor bg, QColor fg, QUndoStack&, data::Nonogram&);

      void setFillMode (data::Answer::Datum);
      void setNonogram (data::Nonogram&);
      void checkAnswer();
      void resetAnswer();

      bool canLock() const;
      bool canUnlock() const;
      void lock();
      void unlock();
      bool isEmpty() const;

      void redo();
      void undo();

      void showSolution (bool animate);
      void showTutorial();

    signals:
      void puzzleChanged();
      void panned (QPointF delta);
      void solved();

    protected:
      void mouseMoveEvent (QMouseEvent*) override;
      void mousePressEvent (QMouseEvent*) override;
      void mouseReleaseEvent (QMouseEvent*) override;
      void paintGL() override;
      void resizeGL (int width, int height) override;

    private:
      void reset();
      data::grid::Cell fromPosition (QRect, QPoint) const;
      data::clues::FullIndex fromCell ( FieldType
                                      , data::Clues::Type
                                      , data::grid::Cell
                                      ) const;
      void checkCell (data::grid::Cell);
      bool fillCell (QPoint, bool first_hit);
      bool crossClue (QPoint, bool first_hit);
      void finishPuzzle();

      QPoint clueCenter (QRect clues_rect, data::grid::Cell) const;
      void drawClue ( QPainter& painter
                    , FieldType type
                    , data::Clues::Type clue_type
                    , data::grid::Cell
                    , data::clues::FullIndex
                    , bool mark_as_error
                    );

      QPoint slotCenter (data::grid::Cell) const;
      void drawCell ( QPainter& painter
                    , data::grid::Cell
                    , data::Answer::Datum
                    );

      void drawClues (QPainter& painter, FieldType);
      void drawPuzzle (QPainter& painter);

      void updateRects (QSize window_size);

      QColor const bg_color_;
      QColor const fg_color_;
      QColor const mistake_color_;
      QColor const locked_color_;
      QUndoStack& undo_stack_;
      data::Nonogram& nonogram_;
      data::Answer::Datum fill_mode_;
      std::size_t font_size_;
      std::size_t slot_size_;
      std::map<FieldType, QRect> field_rects_;
      QRect play_field_rect_;

      QPoint last_pan_position_;
      struct Hit
      {
        FieldType type;
        data::grid::Cell current_slot;
        std::variant<data::ClueState, data::Answer::Datum> data;
      };

      std::optional<data::grid::Cell> current_slot_;
      std::optional<Hit> current_hit_;
      std::optional<data::grid::Cell> current_error_slot_;
      std::vector<QRect> highlights_;
      bool solved_;
    };
  }
}
