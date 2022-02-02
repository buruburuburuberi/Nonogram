#include <nonogram/gui/PlayField.hpp>

#include <nonogram/gui/command/Cross.hpp>
#include <nonogram/gui/command/Fill.hpp>

#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtWidgets/QMessageBox>

namespace nonogram
{
  namespace gui
  {
    PlayField::PlayField (QUndoStack& undo_stack, data::Nonogram nonogram)
    : QOpenGLWidget()
    , undo_stack_ (undo_stack)
    , nonogram_ (std::move (nonogram))
    , fill_mode_ (data::Answer::Datum::Filled)
    , font_size_ (18)
    , slot_size_ (30)
    , solved_ (false)
    {
      setAutoFillBackground (true);
    }

    void PlayField::updateRects (QSize window_size)
    {
      std::size_t const border (2);
      QSize const size_of_left_clues
        ( nonogram_.clueColumns (data::Solution::ClueType::Left).value * slot_size_
        , nonogram_.clueRows (data::Solution::ClueType::Left).value * slot_size_
        );
      QSize const size_of_top_clues
        ( nonogram_.clueColumns (data::Solution::ClueType::Top).value * slot_size_
        , nonogram_.clueRows (data::Solution::ClueType::Top).value * slot_size_
        );
      QSize const size_of_right_clues
        ( nonogram_.clueColumns (data::Solution::ClueType::Right).value * slot_size_
        , nonogram_.clueRows (data::Solution::ClueType::Right).value * slot_size_
        );
      QSize const size_of_bottom_clues
        ( nonogram_.clueColumns (data::Solution::ClueType::Bottom).value * slot_size_
        , nonogram_.clueRows (data::Solution::ClueType::Bottom).value * slot_size_
        );
      QSize const puzzle_size ( nonogram_.dataColumns().value * slot_size_
                              , nonogram_.dataRows().value * slot_size_
                              );

      clues_rects_[data::Solution::ClueType::Left]
        = QRect ( QPoint (0, size_of_top_clues.height() + border)
                , size_of_left_clues
                );
      clues_rects_[data::Solution::ClueType::Top]
        = QRect ( QPoint (size_of_left_clues.width() + border, 0)
                , size_of_top_clues
                );
      puzzle_rect_ =
          QRect ( QPoint ( clues_rects_.at (data::Solution::ClueType::Top).left()
                         , clues_rects_.at (data::Solution::ClueType::Left).top()
                         )
                , puzzle_size
                );
      clues_rects_[data::Solution::ClueType::Right]
        = QRect ( QPoint ( puzzle_rect_.x() + puzzle_rect_.width() + border
                         , clues_rects_.at (data::Solution::ClueType::Left).top()
                         )
                , size_of_right_clues
                );
      clues_rects_[data::Solution::ClueType::Bottom]
        = QRect ( QPoint ( clues_rects_.at (data::Solution::ClueType::Top).left()
                         , puzzle_rect_.y() + puzzle_rect_.height() + border
                         )
                , size_of_bottom_clues
                );
      field_rect_ =
          QRect ( QPoint ( clues_rects_.at (data::Solution::ClueType::Left).x()
                         , clues_rects_.at (data::Solution::ClueType::Top).y()
                         )
                , QPoint ( clues_rects_.at (data::Solution::ClueType::Right).right()
                         , clues_rects_.at (data::Solution::ClueType::Bottom).bottom()
                         )
                );

      QPoint const offset ( (window_size.width() - field_rect_.width()) / 2.0f
                          , (window_size.height() - field_rect_.height()) / 2.0f
                          );
      puzzle_rect_.translate (offset);
      clues_rects_.at (data::Solution::ClueType::Left).translate (offset);
      clues_rects_.at (data::Solution::ClueType::Right).translate (offset);
      clues_rects_.at (data::Solution::ClueType::Top).translate (offset);
      clues_rects_.at (data::Solution::ClueType::Bottom).translate (offset);
      field_rect_.translate (offset);
    }

    void PlayField::setFillMode (data::Answer::Datum mode)
    {
      fill_mode_ = mode;
    }

    void PlayField::reset()
    {
      undo_stack_.clear();
      solved_ = false;
      setDisabled (false);
      current_error_slot_.reset();
    }

    void PlayField::setNonogram (data::Nonogram nonogram)
    {
      nonogram_ = std::move (nonogram);
      updateRects (size());

      setMinimumSize (field_rect_.size() + QSize (50, 50));

      reset();

      update();
    }

    void PlayField::checkAnswer()
    {
      auto const maybe_mistake (nonogram_.findFirstMistake());
      current_error_slot_ = maybe_mistake;
      update();
    }

    void PlayField::checkSlot (data::Slot slot)
    {
      current_error_slot_ = nonogram_.isMistake (slot)
                          ? std::optional (slot)
                          : std::nullopt;
      update();
    }

    void PlayField::resetAnswer()
    {
      nonogram_.resetAnswer();

      reset();

      update();
    }

    void PlayField::redo()
    {
      undo_stack_.redo();
      current_error_slot_.reset();
      update();
    }

    void PlayField::undo()
    {
      undo_stack_.undo();
      current_error_slot_.reset();
      update();
    }

    QPoint PlayField::clueCenter (QRect clues_rect, data::Slot slot) const
    {
      auto const half_slot_size (slot_size_ / 2.f);
      return QPoint
        ( clues_rect.x() + slot.column.value * slot_size_ + half_slot_size
        , clues_rect.y() + slot.row.value * slot_size_ + half_slot_size
        );
    }

    void PlayField::drawClue ( QPainter& painter
                             , data::Solution::ClueType type
                             , data::Slot slot
                             , bool mark_as_error
                             )
    {
      auto const clue_center (clueCenter (clues_rects_.at (type), slot));

      auto const background_size (slot_size_ - 2);
      QRect background_rect (0, 0, background_size, background_size);
      background_rect.moveCenter (clue_center);

      painter.fillRect (background_rect, mark_as_error ? Qt::red : Qt::yellow);

      auto const slot_size
        ( ( current_hit_
         && std::holds_alternative<ClueHit> (current_hit_.value())
         && std::get<ClueHit> (current_hit_.value()).current_slot == slot
          )
        ? slot_size_ - 4
        : slot_size_
        );

      auto const clue (nonogram_.clue (type, slot));

      if (clue > 0)
      {
        painter.setPen (Qt::black);
        auto const text_size (slot_size);
        QFont font;
        font.setPixelSize (font_size_);
        painter.setFont (font);
        QRect text_rect (0, 0, text_size, text_size);
        text_rect.moveCenter (clue_center);
        painter.drawText (text_rect, Qt::AlignCenter, QString::number (clue));

        if (nonogram_.isCrossed (type, slot))
        {
          auto pen (painter.pen());
          pen.setWidth (2);
          painter.setPen (pen);

          auto const cross_size (slot_size - 10);
          QRect cross_rect (0, 0, cross_size, cross_size);
          cross_rect.moveCenter (clue_center);

          painter.drawLine (cross_rect.topLeft(), cross_rect.bottomRight());
          painter.drawLine (cross_rect.bottomLeft(), cross_rect.topRight());
        }
      }
    }

    void PlayField::drawClues (QPainter& painter, data::Solution::ClueType type)
    {
      for ( data::Column column {0}
          ; column.value < nonogram_.clueColumns (type).value
          ; ++column.value
          )
      {
        for ( data::Row row {0}
            ; row.value < nonogram_.clueRows (type).value
            ; ++row.value
            )
        {
          auto const mark_as_error
            ( current_error_slot_
           && ( ( ( type == data::Solution::ClueType::Left
                 || type == data::Solution::ClueType::Right
                  )
               && row.value == current_error_slot_.value().row.value
                )
             || ( ( type == data::Solution::ClueType::Top
                 || type == data::Solution::ClueType::Bottom
                  )
               && column.value == current_error_slot_.value().column.value
                )
              )
            );
          drawClue (painter, type, {column, row}, mark_as_error);
        }
      }
    }

    data::Slot PlayField::fromPosition (QRect rect, QPoint position) const
    {
      return { data::Column {(position.x() - rect.x()) / slot_size_}
             , data::Row {(position.y() - rect.y()) / slot_size_}
             };
    }

    QPoint PlayField::slotCenter (data::Slot slot) const
    {
      auto const half_slot_size (slot_size_ / 2.f);
      return QPoint
        ( puzzle_rect_.x() + slot.column.value * slot_size_ + half_slot_size
        , puzzle_rect_.y() + slot.row.value * slot_size_ + half_slot_size
        );
    }

    void PlayField::drawSlot ( QPainter& painter
                             , data::Slot slot
                             , data::Answer::Datum datum
                             )
    {
      auto const slot_center (slotCenter (slot));

      auto const background_size (slot_size_ - 2);
      QRect background_rect (0, 0, background_size, background_size);
      background_rect.moveCenter (slot_center);

      painter.fillRect
        ( background_rect
        ,  current_error_slot_ && current_error_slot_.value() == slot
          ? Qt::red
          : Qt::yellow
        );

      if ( solved_
        && datum != data::Answer::Datum::Filled
        && datum != data::Answer::Datum::Empty
         )
      {
        return;
      }

      painter.setBrush (Qt::black);
      auto pen (painter.pen());
      pen.setWidth (2);
      pen.setColor (Qt::black);
      painter.setPen (pen);

      auto const slot_size
        ( ( current_hit_
         && std::holds_alternative<DataHit> (current_hit_.value())
         && std::get<DataHit> (current_hit_.value()).current_slot == slot
          )
        ? slot_size_ - 4
        : slot_size_
        );

      switch (datum)
      {
        case data::Answer::Datum::Empty:
        {
          break;
        }
        case data::Answer::Datum::Filled:
        {
          auto const point_size (solved_ ? slot_size_ : slot_size - 6);
          QRect point_rect (0, 0, point_size, point_size);
          point_rect.moveCenter (slot_center);

          painter.fillRect (point_rect, painter.pen().color());
          break;
        }
        case data::Answer::Datum::Crossed:
        {
          auto const cross_size (slot_size - 8);
          QRect cross_rect (0, 0, cross_size, cross_size);
          cross_rect.moveCenter (slot_center);

          painter.drawLine (cross_rect.topLeft(), cross_rect.bottomRight());
          painter.drawLine (cross_rect.bottomLeft(), cross_rect.topRight());
          break;
        }
        case data::Answer::Datum::FillMark:
        {
          auto const fill_mark_size ((slot_size - 8.0f) / 2.0f);
          QRect fill_mark_rect (0, 0, fill_mark_size, fill_mark_size);
          fill_mark_rect.moveCenter (slot_center);

          painter.drawEllipse (fill_mark_rect);
          break;
        }
        case data::Answer::Datum::CrossMark:
        {
          auto const cross_mark_size (slot_size - 8);
          QRect cross_mark_rect (0, 0, cross_mark_size, cross_mark_size);
          cross_mark_rect.moveCenter (slot_center);

          painter.drawLine ( cross_mark_rect.bottomLeft()
                           , cross_mark_rect.topRight()
                           );
          break;
        }
        default:
        {
          throw std::invalid_argument ("Unknown datum type.");
        }
      }
    }

    void PlayField::drawPuzzle (QPainter& painter)
    {
      for ( data::Column column {0}
          ; column.value < nonogram_.dataColumns().value
          ; ++column.value
          )
      {
        for ( data::Row row {0}
            ; row.value < nonogram_.dataRows().value
            ; ++row.value
            )
        {
          data::Slot const slot {column, row};
          drawSlot (painter, slot, nonogram_.at (slot));
        }
      }
    }

    bool PlayField::fillSlot (QPoint position, bool first_hit)
    {
      if (puzzle_rect_.contains (position))
      {
        auto const slot (fromPosition (puzzle_rect_, position));

        auto const current_datum (nonogram_.at (slot));

        if (!current_hit_)
        {
          current_hit_.emplace
            ( DataHit
                { slot
                , current_datum == fill_mode_
                  ? data::Answer::Datum::Empty
                  : fill_mode_
                }
            );
        }

        auto const to_fill (std::get<DataHit> (current_hit_.value()).datum);
        if ( current_datum == to_fill
          || ( (to_fill == data::Answer::Datum::Empty)
            && current_datum != fill_mode_
             )
           )
        {
          return true;
        }

        data::Slots const data_slots {slot};
        if (first_hit)
        {
          undo_stack_.push
            ( command::Fill::start
                (nonogram_, data_slots, current_datum, to_fill)
            );
        }
        else
        {
          undo_stack_.push
            ( command::Fill::append
                (nonogram_, data_slots, current_datum, to_fill)
            );
        }

        current_hit_.emplace (DataHit {slot, to_fill});

        if (current_error_slot_ == slot)
        {
          checkSlot (current_error_slot_.value());
        }

        if ( ( nonogram_.at (slot) == data::Answer::Datum::Filled
            || nonogram_.at (slot) == data::Answer::Datum::Empty
             )
          && nonogram_.isSolved()
           )
        {
          QTimer::singleShot (0, this, &PlayField::finishPuzzle);
        }

        update();

        return true;
      }

      return false;
    }

    bool PlayField::crossClue (QPoint position, bool first_hit)
    {
      auto cross_clue
        ( [&] (data::Solution::ClueType type, QRect rect) -> bool
          {
            if (!rect.contains (position))
            {
              return false;
            }

            auto const slot (fromPosition (rect, position));

            auto const state
              ( current_hit_
              ? std::get<ClueHit> (current_hit_.value()).state
              : !nonogram_.isCrossed (type, slot)
              );

            auto const current_state (nonogram_.isCrossed (type, slot));

            if (current_state == state)
            {
              return true;
            }

            current_hit_.emplace (ClueHit {type, slot, state});

            data::Solution::ClueSlots const clue_slots {{type, {slot}}};
            if (first_hit)
            {
              undo_stack_.push
                ( command::Cross::start
                    (nonogram_, clue_slots, current_state, state)
                );
            }
            else
            {
              undo_stack_.push
                ( command::Cross::append
                    (nonogram_, clue_slots, current_state, state)
                );
            }

            update();

            return true;
          }
        );

      if (current_hit_)
      {
        auto const type (std::get<ClueHit> (current_hit_.value()).type);
        return cross_clue (type, clues_rects_.at (type));
      }
      else
      {
        for (auto const& rect : clues_rects_)
        {
          if (cross_clue (rect.first, rect.second))
          {
            return true;
          }
        }
      }

      return false;
    }

    void PlayField::finishPuzzle()
    {
      QMessageBox::information ( this
                               , "Congratulations!"
                               , "You have solved this puzzle."
                               );
      solved_ = true;
      setDisabled (true);

      update();

      emit solved();
    }

    void PlayField::mouseMoveEvent (QMouseEvent* event)
    {
      if (event->buttons() & Qt::LeftButton)
      {
        if (current_hit_)
        {
          if (std::holds_alternative<DataHit> (current_hit_.value()))
          {
            fillSlot (event->pos(), false);
          }
          else if (std::holds_alternative<ClueHit> (current_hit_.value()))
          {
            crossClue (event->pos(), false);
          }
        }
      }
    }

    void PlayField::mousePressEvent (QMouseEvent* event)
    {
      if (event->buttons() & Qt::LeftButton)
      {
        if (fillSlot (event->pos(), true))
        {
          return;
        }
        else if (crossClue (event->pos(), true))
        {
          return;
        }
      }
    }

    void PlayField::mouseReleaseEvent (QMouseEvent*)
    {
      if (current_hit_)
      {
        current_hit_.reset();

        update();
      }
    }

    void PlayField::paintGL()
    {
      QPainter painter (this);
      painter.fillRect (rect(), Qt::black);

      drawClues (painter, data::Solution::ClueType::Left);
      drawClues (painter, data::Solution::ClueType::Top);
      drawClues (painter, data::Solution::ClueType::Right);
      drawClues (painter, data::Solution::ClueType::Bottom);
      drawPuzzle (painter);
    }

    void PlayField::resizeGL (int width, int height)
    {
      updateRects (QSize (width, height));

      update();
    }
  };
}
