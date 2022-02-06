#include <nonogram/gui/PlayField.hpp>

#include <nonogram/gui/command/Cross.hpp>
#include <nonogram/gui/command/Fill.hpp>
#include <nonogram/gui/command/Lock.hpp>
#include <nonogram/gui/painting.hpp>

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtWidgets/QMessageBox>

namespace nonogram
{
  namespace gui
  {
    static std::array<PlayField::FieldType, 5> constexpr all_field_types
    { PlayField::FieldType::LeftClues
    , PlayField::FieldType::RightClues
    , PlayField::FieldType::TopClues
    , PlayField::FieldType::BottomClues
    , PlayField::FieldType::Puzzle
    };

    PlayField::PlayField ( QColor bg
                         , QColor fg
                         , QUndoStack& undo_stack
                         , data::Nonogram nonogram
                         )
    : QOpenGLWidget()
    , bg_color_ (bg)
    , fg_color_ (fg)
    , mistake_color_ (Qt::red)
    , locked_color_ (Qt::gray)
    , undo_stack_ (undo_stack)
    , nonogram_ (std::move (nonogram))
    , fill_mode_ (data::Answer::Datum::Filled)
    , font_size_ (16)
    , slot_size_ (24)
    , solved_ (false)
    {
      setAutoFillBackground (true);

      updateRects (size());
    }

    void PlayField::updateRects (QSize window_size)
    {
      std::size_t const puzzle_border (4);

      QSize const puzzle_size
        ( nonogram_.dataColumns().value * slot_size_
        , nonogram_.dataRows().value * slot_size_
        );
      QSize const row_clue_size
        ( nonogram_.maxNumberOfClues (data::Clues::Type::Row).value  * slot_size_
        , puzzle_size.height()
        );
      QSize const column_clue_size
        ( puzzle_size.width()
        , nonogram_.maxNumberOfClues (data::Clues::Type::Column).value  * slot_size_
        );

      field_rects_[FieldType::LeftClues] =
        { QPoint (0, column_clue_size.height() + puzzle_border)
        , row_clue_size
        };
      field_rects_[FieldType::TopClues] =
        { QPoint (row_clue_size.width() + puzzle_border, 0)
        , column_clue_size
        };
      field_rects_[FieldType::Puzzle] =
        { QPoint ( field_rects_.at (FieldType::TopClues).left()
                 , field_rects_.at (FieldType::LeftClues).top()
                 )
        , puzzle_size
        };
      field_rects_[FieldType::RightClues] =
        { QPoint ( field_rects_.at (FieldType::Puzzle).x()
                   + field_rects_.at (FieldType::Puzzle).width() + puzzle_border
                 , field_rects_.at (FieldType::LeftClues).top()
                 )
        , row_clue_size
        };
      field_rects_[FieldType::BottomClues] =
        { QPoint ( field_rects_.at (FieldType::TopClues).left()
                 , field_rects_.at (FieldType::Puzzle).y()
                   + field_rects_.at (FieldType::Puzzle).height() + puzzle_border
                 )
        , column_clue_size
        };
      play_field_rect_ =
        { QPoint ( field_rects_.at (FieldType::LeftClues).x()
                 , field_rects_.at (FieldType::TopClues).y()
                 )
        , QSize ( field_rects_.at (FieldType::RightClues).right() + 50
                , field_rects_.at (FieldType::BottomClues).bottom() + 50
                )
        };

      QSize new_size
        ( std::max (window_size.width(), play_field_rect_.width())
        , std::max (window_size.height(), play_field_rect_.height())
        );

      QPoint const offset
        ( (new_size.width() - play_field_rect_.width()) / 2.0f + 25
        , (new_size.height() - play_field_rect_.height()) / 2.0f + 10
        );
      for (auto const& type : all_field_types)
      {
        field_rects_.at (type).translate (offset);
      }
      play_field_rect_.translate (offset);

      setMinimumSize (new_size);
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

    bool PlayField::canLock() const
    {
      return nonogram_.canLock();
    }

    bool PlayField::canUnlock() const
    {
      return nonogram_.canUnlock();
    }

    void PlayField::lock()
    {
      undo_stack_.push (command::Lock::lock (nonogram_));

      update();
    }

    void PlayField::unlock()
    {
      undo_stack_.push (command::Lock::unlock (nonogram_));

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
                             , FieldType field_type
                             , data::Clues::Type clue_type
                             , data::Slot slot
                             , data::FullIndex full_index
                             , bool mark_as_error
                             )
    {
      auto const clue_center (clueCenter (field_rects_.at (field_type), slot));

      auto const background_size (slot_size_ - 2);
      QRect background_rect (0, 0, background_size, background_size);
      background_rect.moveCenter (clue_center);

      painter.fillRect ( background_rect
                       , mark_as_error ? mistake_color_ : bg_color_
                       );

      if ( full_index.minor.value
           < nonogram_.clueMinorSize (clue_type, full_index.main).value
         )
      {
        auto const slot_size
          ( ( current_hit_
           && std::holds_alternative<data::ClueState> (current_hit_->data)
           && current_hit_->current_slot == slot
            )
          ? slot_size_ - 4
          : slot_size_
          );

        auto const clue (nonogram_.clue (clue_type, full_index));
        QColor const color
            ( nonogram_.isClueLocked (clue_type, full_index)
            ? locked_color_
            : fg_color_
            );
        painter.setPen (color);

        auto const text_size (slot_size);
        QFont font;
        font.setPixelSize (font_size_);
        painter.setFont (font);
        QRect text_rect (0, 0, text_size, text_size);
        text_rect.moveCenter (clue_center);
        painter.drawText ( text_rect, Qt::AlignCenter
                         , QString::number (clue)
                         );

        if (nonogram_.isCrossed (clue_type, full_index))
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

    void PlayField::drawClues (QPainter& painter, FieldType field_type)
    {
      auto const clue_type
        ( field_type == FieldType::LeftClues
          || field_type == FieldType::RightClues
        ? data::Clues::Type::Row
        : data::Clues::Type::Column
        );

      auto const max_number_of_clues
        (nonogram_.maxNumberOfClues (clue_type).value);

      data::Columns const columns
        { clue_type == data::Clues::Type::Row
        ? max_number_of_clues
        : nonogram_.clueMainSize (clue_type).value
        };
      data::Rows const rows
        { clue_type == data::Clues::Type::Row
        ? nonogram_.clueMainSize (clue_type).value
        : max_number_of_clues
        };

      for (data::Row row {0}; row.value < rows.value; ++row.value)
      {
        for ( data::Column column {0}
            ; column.value < columns.value
            ; ++column.value
            )
        {
          auto const mark_as_error
            ( current_error_slot_
           && ( ( (clue_type == data::Clues::Type::Row)
               && row.value == current_error_slot_.value().row.value
                )
             || ( (clue_type == data::Clues::Type::Column)
               && column.value == current_error_slot_.value().column.value
                )
              )
            );

          data::Slot const slot {column, row};
          data::FullIndex const full_index
            (fromSlot (field_type, clue_type, slot));

          drawClue
            (painter, field_type, clue_type, slot, full_index, mark_as_error);
        }
      }
    }

    data::Slot PlayField::fromPosition (QRect rect, QPoint position) const
    {
      return { data::Column {(position.x() - rect.x()) / slot_size_}
             , data::Row {(position.y() - rect.y()) / slot_size_}
             };
    }

    data::FullIndex PlayField::fromSlot ( FieldType field_type
                                        , data::Clues::Type clue_type
                                        , data::Slot slot
                                        ) const
    {
      auto const max_number_of_clues
        (nonogram_.maxNumberOfClues (clue_type).value);
      data::FullIndex full_index
        (data::Clues::toFullIndex (clue_type, slot));

      if (field_type == FieldType::LeftClues || field_type == FieldType::TopClues)
      {
        auto const offset
          ( max_number_of_clues
            - nonogram_.clueMinorSize (clue_type, full_index.main).value
          );
        full_index.minor.value -= offset;
      }

      return full_index;
    }

    QPoint PlayField::slotCenter (data::Slot slot) const
    {
      auto const half_slot_size (slot_size_ / 2.f);
      return QPoint
        ( field_rects_.at (FieldType::Puzzle).x()
          + slot.column.value * slot_size_ + half_slot_size
        , field_rects_.at (FieldType::Puzzle).y()
          + slot.row.value * slot_size_ + half_slot_size
        );
    }

    void PlayField::drawSlot ( QPainter& painter
                             , data::Slot slot
                             , data::Answer::Datum datum
                             )
    {
      auto const slot_center (slotCenter (slot));

      QRect background_rect (0, 0, slot_size_, slot_size_);
      background_rect.moveCenter (slot_center);
      drawBackground
        ( painter
        , background_rect
        , current_error_slot_ && current_error_slot_.value() == slot
          ? mistake_color_
          : bg_color_
        );

      if ( solved_
        && datum != data::Answer::Datum::Filled
        && datum != data::Answer::Datum::Empty
         )
      {
        return;
      }

      QColor const color
        (nonogram_.isDatumLocked (slot) ? locked_color_ : fg_color_);

      auto const slot_size
        ( ( !solved_
         && current_hit_
         && std::holds_alternative<data::Answer::Datum> (current_hit_->data)
         && current_hit_->current_slot == slot
          )
        ? slot_size_ - 4
        : slot_size_
        );

      QRect icon_rect (0, 0, slot_size, slot_size);
      icon_rect.moveCenter (slot_center);

      switch (datum)
      {
        case data::Answer::Datum::Empty:
        {
          break;
        }
        case data::Answer::Datum::Filled:
        {
          drawFill (painter, icon_rect, color, solved_);
          break;
        }
        case data::Answer::Datum::Crossed:
        {
          drawCross (painter, icon_rect, color);
          break;
        }
        case data::Answer::Datum::FillMark:
        {
          drawFillMark (painter, icon_rect, color);
          break;
        }
        case data::Answer::Datum::CrossMark:
        {
          drawCrossMark (painter, icon_rect, color);
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
          drawSlot (painter, slot, nonogram_.answer (slot));
        }
      }
    }

    bool PlayField::fillSlot (QPoint position, bool first_hit)
    {
      if (field_rects_.at (FieldType::Puzzle).contains (position))
      {
        auto const slot (fromPosition (field_rects_.at (FieldType::Puzzle), position));

        if (nonogram_.isDatumLocked (slot))
        {
          return true;
        }

        auto const current_datum (nonogram_.answer (slot));

        if (!current_hit_)
        {
          current_hit_ =
            { FieldType::Puzzle
            , slot
            , current_datum == fill_mode_
              ? data::Answer::Datum::Empty
              : fill_mode_
            };
        }

        auto const to_fill (std::get<data::Answer::Datum> (current_hit_->data));
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

        current_hit_ = {FieldType::Puzzle, slot, to_fill};

        if (current_error_slot_ == slot)
        {
          checkSlot (current_error_slot_.value());
        }

        if ( ( nonogram_.answer (slot) == data::Answer::Datum::Filled
            || nonogram_.answer (slot) == data::Answer::Datum::Empty
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
        ( [&] (FieldType field_type, QRect rect) -> bool
          {
            auto const clue_type
              ( field_type == FieldType::LeftClues
                || field_type == FieldType::RightClues
              ? data::Clues::Type::Row
              : data::Clues::Type::Column
              );

            if (!rect.contains (position))
            {
              return false;
            }

            auto const slot (fromPosition (rect, position));
            data::FullIndex const full_index
              (fromSlot (field_type, clue_type, slot));

            if ( full_index.minor.value
              >= nonogram_.clueMinorSize (clue_type, full_index.main).value
               )
            {
              return true;
            }

            if (nonogram_.isClueLocked (clue_type, full_index))
            {
              return true;
            }

            auto const current_state
              (nonogram_.isCrossed (clue_type, full_index));

            auto const state
              ( current_hit_
              ? std::get<data::ClueState> (current_hit_->data)
              : !current_state
              );

            if (current_state == state)
            {
              return true;
            }

            current_hit_ = {field_type, slot, state};

            data::Solution::ClueIndices const clue_indices
              {{clue_type, {full_index}}};
            if (first_hit)
            {
              undo_stack_.push
                ( command::Cross::start
                    (nonogram_, clue_indices, current_state, state)
                );
            }
            else
            {
              undo_stack_.push
                ( command::Cross::append
                    (nonogram_, clue_indices, current_state, state)
                );
            }

            update();

            return true;
          }
        );

      if (current_hit_)
      {
        auto const type (current_hit_->type);
        return cross_clue (type, field_rects_.at (type));
      }
      else
      {
        for (auto const& rect : field_rects_)
        {
          if (rect.first != FieldType::Puzzle)
          {
            if (cross_clue (rect.first, rect.second))
            {
              return true;
            }
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
          if (std::holds_alternative<data::Answer::Datum> (current_hit_->data))
          {
            fillSlot (event->pos(), false);
          }
          else if (std::holds_alternative<data::ClueState> (current_hit_->data))
          {
            crossClue (event->pos(), false);
          }
        }
      }
      else if (event->buttons() & Qt::RightButton)
      {
        auto const current_position (event->pos());
        auto const delta (current_position - last_pan_position_);
        emit panned ( { delta.x() / static_cast<float> (width())
                      , delta.y() / static_cast<float> (height())
                      }
                    );

        last_pan_position_ = current_position;
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
      else if (event->buttons() & Qt::RightButton)
      {
        setCursor (Qt::ClosedHandCursor);
        last_pan_position_ = event->pos();
      }
    }

    void PlayField::mouseReleaseEvent (QMouseEvent*)
    {
      setCursor (Qt::ArrowCursor);

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

      for (auto const& type : all_field_types)
      {
        if (type == FieldType::Puzzle)
        {
          drawPuzzle (painter);
        }
        else
        {
          drawClues (painter, type);
        }
      }

      painter.setPen (Qt::white);
      painter.drawText (QPoint (10, height() - 10), "by Delger Lhamsuren");
    }

    void PlayField::resizeGL (int width, int height)
    {
      updateRects (QSize (width, height));

      update();
    }

    void PlayField::showSolution (bool animate)
    {
      setDisabled (true);

      for (data::Row row {0}; row.value < nonogram_.dataRows().value; ++row.value)
      {
        for ( data::Column column {0}
            ; column.value < nonogram_.dataColumns().value
            ; ++column.value
            )
        {
          data::Slot const slot {column, row};

          if (nonogram_.solution (slot))
          {
            nonogram_.fill (slot, data::Answer::Datum::Filled);

            if (animate)
            {
              update();
              QCoreApplication::processEvents
                (QEventLoop::ExcludeUserInputEvents);
            }
          }
        }
      }

      solved_ = true;

      update();
    }
  };
}
