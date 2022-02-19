#include <nonogram/gui/PlayField.hpp>

#include <nonogram/data/grid/Column.hpp>
#include <nonogram/data/grid/Row.hpp>
#include <nonogram/gui/painting.hpp>
#include <nonogram/gui/command/Cross.hpp>
#include <nonogram/gui/command/Fill.hpp>
#include <nonogram/gui/command/Lock.hpp>
#include <nonogram/util/CenteredRect.hpp>
#include <nonogram/util/optional_if.hpp>
#include <nonogram/util/unique_qt_ptr.hpp>

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QToolTip>
#include <QtWidgets/QVBoxLayout>

#include <array>

namespace nonogram::gui
{
  std::array<PlayField::FieldType, 5> constexpr all_field_types
  { PlayField::FieldType::LeftClues
  , PlayField::FieldType::RightClues
  , PlayField::FieldType::TopClues
  , PlayField::FieldType::BottomClues
  , PlayField::FieldType::Puzzle
  };

  PlayField::PlayField ( QColor bg
                       , QColor fg
                       , QUndoStack& undo_stack
                       , data::Nonogram& nonogram
                       )
  : QOpenGLWidget()
  , bg_color_ (bg)
  , fg_color_ (fg)
  , mistake_color_ (Qt::red)
  , locked_color_ (Qt::gray)
  , undo_stack_ (undo_stack)
  , nonogram_ (nonogram)
  , fill_mode_ (data::Answer::Datum::Filled)
  , font_size_ (18)
  , slot_size_ (30)
  , solved_ (false)
  {
    setAutoFillBackground (true);
    setMouseTracking (true);

    updateRects (size());
  }

  void PlayField::updateRects (QSize window_size)
  {
    std::size_t const puzzle_border (2);

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
      , QSize ( field_rects_.at (FieldType::RightClues).right()
              , field_rects_.at (FieldType::BottomClues).bottom()
              )
      };

    QRect const play_field_rect_with_offset
      (play_field_rect_.topLeft(), play_field_rect_.size() + QSize (50, 50));

    QPoint const offset
      ( (window_size.width() - play_field_rect_with_offset.width()) / 2.0f + 25
      , (window_size.height() - play_field_rect_with_offset.height()) / 2.0f + 10
      );
    for (auto const& type : all_field_types)
    {
      field_rects_.at (type).translate (offset);
    }
    play_field_rect_.translate (offset);

    setMinimumSize (play_field_rect_with_offset.size());
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
    current_slot_.reset();
  }

  void PlayField::setNonogram (data::Nonogram& nonogram)
  {
    nonogram_ = nonogram;
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

  void PlayField::checkCell (data::grid::Cell cell)
  {
    current_error_slot_ = util::optional_if (nonogram_.isMistake (cell), cell);
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

  bool PlayField::isEmpty() const
  {
    return nonogram_.isEmpty();
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

  QPoint PlayField::clueCenter (QRect clues_rect, data::grid::Cell cell) const
  {
    auto const half_slot_size (slot_size_ / 2.f);
    return QPoint
      ( clues_rect.x() + cell.column.value * slot_size_ + half_slot_size
      , clues_rect.y() + cell.row.value * slot_size_ + half_slot_size
      );
  }

  void PlayField::drawClue ( QPainter& painter
                           , FieldType field_type
                           , data::Clues::Type clue_type
                           , data::grid::Cell cell
                           , data::clues::FullIndex full_index
                           )
  {
    QPoint const clue_center (clueCenter (field_rects_.at (field_type), cell));

    auto const background_size (slot_size_ - 2);
    util::CenteredRect const background_rect
      (clue_center, QSize (background_size, background_size));

    bool const mark_as_error
      ( current_error_slot_
     && ( ( (clue_type == data::Clues::Type::Row)
         && cell.row == current_error_slot_.value().row
          )
       || ( (clue_type == data::Clues::Type::Column)
         && cell.column == current_error_slot_.value().column
          )
        )
      );

    painter.fillRect ( background_rect
                     , mark_as_error ? mistake_color_ : bg_color_
                     );

    if (full_index.minor < nonogram_.clueMinorSize (clue_type, full_index.main))
    {
      auto const slot_size
        ( ( current_hit_
         && std::holds_alternative<data::ClueState> (current_hit_->data)
         && current_hit_->current_slot == cell
          )
        ? slot_size_ - 4
        : slot_size_
        );

      auto const clue (nonogram_.clue (clue_type, full_index));
      QColor const color
          ( ( !nonogram_.id().internal()
           && nonogram_.isClueLocked (clue_type, full_index)
            )
            ? locked_color_
            : fg_color_
          );
      painter.setPen (color);

      auto const text_size (slot_size);
      QFont font;
      font.setPixelSize (font_size_);
      painter.setFont (font);
      painter.drawText
        ( util::CenteredRect (clue_center, QSize (text_size, text_size))
        , Qt::AlignCenter
        , QString::number (clue)
        );

      if (!solved_ && nonogram_.isCrossed (clue_type, full_index))
      {
        drawCross
          ( painter
          , util::CenteredRect (clue_center, QSize (slot_size, slot_size))
          , color
          );
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

    data::grid::Column const columns
      { clue_type == data::Clues::Type::Row
      ? max_number_of_clues
      : nonogram_.clueMainSize (clue_type).value
      };
    data::grid::Row const rows
      { clue_type == data::Clues::Type::Row
      ? nonogram_.clueMainSize (clue_type).value
      : max_number_of_clues
      };

    for (data::grid::Row row {0}; row < rows; ++row)
    {
      for (data::grid::Column column {0}; column < columns; ++column)
      {
        data::grid::Cell const cell {column, row};
        data::clues::FullIndex const full_index
          (fromCell (field_type, clue_type, cell));

        drawClue (painter, field_type, clue_type, cell, full_index);
      }
    }
  }

  data::grid::Cell PlayField::fromPosition (QRect rect, QPoint position) const
  {
    return { data::grid::Column {(position.x() - rect.x()) / slot_size_}
           , data::grid::Row {(position.y() - rect.y()) / slot_size_}
           };
  }

  data::clues::FullIndex PlayField::fromCell ( FieldType field_type
                                             , data::Clues::Type clue_type
                                             , data::grid::Cell cell
                                             ) const
  {
    auto const max_number_of_clues (nonogram_.maxNumberOfClues (clue_type));
    data::clues::FullIndex full_index
      (data::Clues::toFullIndex (clue_type, cell));

    if (field_type == FieldType::LeftClues || field_type == FieldType::TopClues)
    {
      auto const offset
        ( max_number_of_clues
          - nonogram_.clueMinorSize (clue_type, full_index.main)
        );
      full_index.minor = full_index.minor - offset;
    }

    return full_index;
  }

  QPoint PlayField::slotCenter (data::grid::Cell cell) const
  {
    auto const half_slot_size (slot_size_ / 2.f);
    return QPoint
      ( field_rects_.at (FieldType::Puzzle).x()
        + cell.column.value * slot_size_ + half_slot_size
      , field_rects_.at (FieldType::Puzzle).y()
        + cell.row.value * slot_size_ + half_slot_size
      );
  }

  void PlayField::drawCell ( QPainter& painter
                           , data::grid::Cell cell
                           , data::Answer::Datum datum
                           )
  {
    auto const slot_center (slotCenter (cell));

    drawBackground
      ( painter
      , util::CenteredRect (slot_center, QSize (slot_size_, slot_size_))
      , current_error_slot_ && current_error_slot_.value() == cell
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
      ( ( !nonogram_.id().internal()
       && nonogram_.isDatumLocked (cell)
       && !solved_
        )
      ? locked_color_
      : fg_color_
      );

    auto const slot_size
      ( ( !solved_
       && current_hit_
       && std::holds_alternative<data::Answer::Datum> (current_hit_->data)
       && current_hit_->current_slot == cell
        )
      ? slot_size_ - 4
      : slot_size_
      );

    util::CenteredRect const icon_rect
      (slot_center, QSize (slot_size, slot_size));

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
    for (auto const& [cell, value] : nonogram_.data())
    {
      drawCell (painter, cell, value);
    }
  }

  bool PlayField::fillCell (QPoint position)
  {
    if (field_rects_.at (FieldType::Puzzle).contains (position))
    {
      auto const cell
        (fromPosition (field_rects_.at (FieldType::Puzzle), position));

      if (nonogram_.isDatumLocked (cell))
      {
        return true;
      }

      auto const current_datum (nonogram_.datum (cell));

      bool const first_hit (!current_hit_.has_value());
      if (!current_hit_)
      {
        current_hit_ =
          { FieldType::Puzzle
          , cell
          , current_datum == fill_mode_ ? data::Answer::Datum::Empty : fill_mode_
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

      data::grid::Cells const data_slots {cell};
      if (first_hit)
      {
        undo_stack_.push
          (command::Fill::start (nonogram_, data_slots, current_datum, to_fill));
      }
      else
      {
        undo_stack_.push
          (command::Fill::append (nonogram_, data_slots, current_datum, to_fill));
      }

      current_hit_ = {FieldType::Puzzle, cell, to_fill};

      if (current_error_slot_ == cell)
      {
        checkCell (current_error_slot_.value());
      }

      if ( ( nonogram_.datum (cell) == data::Answer::Datum::Filled
          || nonogram_.datum (cell) == data::Answer::Datum::Empty
           )
        && nonogram_.isSolved()
         )
      {
        QTimer::singleShot (0, this, &PlayField::finishPuzzle);
      }

      update();

      emit puzzleChanged();

      return true;
    }

    return false;
  }

  bool PlayField::crossClue (QPoint position)
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

          auto const cell (fromPosition (rect, position));
          data::clues::FullIndex const full_index
            (fromCell (field_type, clue_type, cell));

          if ( full_index.minor
            >= nonogram_.clueMinorSize (clue_type, full_index.main)
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

          bool const first_hit (!current_hit_.has_value());
          current_hit_ = {field_type, cell, state};

          data::Solution::ClueIndices const indices {{clue_type, {full_index}}};
          if (first_hit)
          {
            undo_stack_.push
              (command::Cross::start (nonogram_, indices, current_state, state));
          }
          else
          {
            undo_stack_.push
              (command::Cross::append (nonogram_, indices, current_state, state));
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
      for (auto const& [type, rect] : field_rects_)
      {
        if ((type != FieldType::Puzzle) && cross_clue (type, rect))
        {
          return true;
        }
      }
    }

    return false;
  }

  void PlayField::finishPuzzle()
  {
    nonogram_.fillClueLocks (false);
    nonogram_.fillDataLocks (false);
    current_slot_.reset();
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
          fillCell (event->pos());
        }
        else if (std::holds_alternative<data::ClueState> (current_hit_->data))
        {
          crossClue (event->pos());
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

    if (event->buttons() != Qt::RightButton)
    {
      current_slot_ = util::optional_if
          ( field_rects_.at (FieldType::Puzzle).contains (event->pos())
          , fromPosition (field_rects_.at (FieldType::Puzzle), event->pos())
          );

      auto const text
        ( current_slot_
        ? QString ("Position: (%1, %2)")
            .arg (current_slot_->column.value + 1)
            .arg (current_slot_->row.value + 1)
        : QString()
        );
      QToolTip::showText (event->globalPos(), text, this, rect());
      update();
    }
  }

  void PlayField::mousePressEvent (QMouseEvent* event)
  {
    if (event->buttons() & Qt::LeftButton)
    {
      if (fillCell (event->pos()))
      {
        return;
      }
      else if (crossClue (event->pos()))
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
    painter.fillRect (rect(), Qt::darkGray);

    for (auto const& [type, rect] : field_rects_)
    {
      painter.fillRect (rect, fg_color_);
    }

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

    auto pen (painter.pen());
    pen.setWidth (3);
    pen.setColor (fg_color_);
    painter.setPen (pen);

    for (auto const& [type, rect] : field_rects_)
    {
      painter.drawRect (rect);
    }

    auto const offset (5 * slot_size_);
    if (nonogram_.dataColumns().value > 5)
    {
      for ( int x (field_rects_.at (FieldType::Puzzle).left())
          ; x < field_rects_.at (FieldType::Puzzle).right()
          ; x += offset
          )
      {
        painter.drawLine ( x, play_field_rect_.top()
                         , x, play_field_rect_.bottom()
                         );
      }
    }

    if (nonogram_.dataRows().value > 5)
    {
      for ( int y (field_rects_.at (FieldType::Puzzle).top())
          ; y < field_rects_.at (FieldType::Puzzle).bottom()
          ; y += offset
          )
      {
        painter.drawLine ( play_field_rect_.left(), y
                         , play_field_rect_.right(), y
                         );
      }
    }

    if (current_slot_)
    {
      auto pen (painter.pen());
      pen.setWidth (4);
      painter.setPen (pen);

      auto const slot_center (slotCenter (current_slot_.value()));
      painter.drawRect
        ( util::CenteredRect
            ( QPoint (play_field_rect_.center().x(), slot_center.y())
            , QSize (play_field_rect_.width(), slot_size_)
            )
        );

      painter.drawRect
        ( util::CenteredRect
            ( QPoint (slot_center.x(), play_field_rect_.center().y())
            , QSize (slot_size_, play_field_rect_.height())
            )
        );
    }

    painter.setPen (Qt::white);
    painter.drawText (QPoint (10, height() - 10), "by Delger Lhamsuren");

    for (auto const& rect : highlights_)
    {
      auto pen (painter.pen());
      pen.setWidth (4);
      pen.setColor (Qt::red);
      painter.setPen (pen);
      painter.drawRect (rect);
    }
  }

  void PlayField::resizeGL (int width, int height)
  {
    updateRects (QSize (width, height));

    update();
  }

  void PlayField::showSolution (Animate animate)
  {
    setDisabled (true);

    for (auto const& [cell, value] : nonogram_.data())
    {
      if (nonogram_.solution (cell))
      {
        nonogram_.fillData (cell, data::Answer::Datum::Filled);

        if (animate == Animate::Yes)
        {
          update();
          QCoreApplication::processEvents (QEventLoop::ExcludeUserInputEvents);
        }
      }
    }

    solved_ = true;

    update();
  }

  void PlayField::showTutorial()
  {
    auto showMessageBox
    ( [&] (QString text, QPoint position)
      {
        QDialog message_box;
        message_box.setWindowFlags (Qt::FramelessWindowHint);
        message_box.setWindowModality (Qt::ApplicationModal);
        util::unique_qt_ptr<QVBoxLayout> main_layout;
        message_box.setFixedSize (400,200);
        message_box.move (pos() + position);
        util::unique_qt_ptr<QLabel> message_label (text);
        message_label->setAlignment (Qt::AlignTop);
        message_label->setTextFormat (Qt::RichText);
        message_label->setWordWrap (true);
        util::unique_qt_ptr<QDialogButtonBox> button_box
          (QDialogButtonBox::Ok);
        main_layout->addWidget (message_label.release());
        main_layout->addWidget (button_box.release());
        message_box.setLayout (main_layout.release());
        connect ( button_box.get(), &QDialogButtonBox::accepted
                , &message_box, &QDialog::accept
                );
        return message_box.exec();
      }
    );

    auto waitUntilCellsAreFilled
    ( [&] (data::grid::Cells slots_to_fill, data::Answer::Datum datum)
      {
        nonogram_.lockData (slots_to_fill, false);

        QEventLoop loop;
        auto connection
          ( connect
              ( this
              , &PlayField::puzzleChanged
              , this
              , [&]
                {
                  if ( std::all_of
                         ( slots_to_fill.begin()
                         , slots_to_fill.end()
                         , [&] (data::grid::Cell cell)
                           {
                             return nonogram_.datum (cell) == datum;
                           }
                         )
                     )
                  {
                    loop.quit();
                  }
                }
              )
          );

        loop.exec();
        this->disconnect (connection);
        if (current_hit_)
        {
          current_hit_.reset();
        }
      }
    );

    QPoint const position (50, height()/2);
    auto const slot_size (static_cast<int> (slot_size_));
    auto const top_rect (field_rects_.at (FieldType::TopClues));
    auto const bottom_rect (field_rects_.at (FieldType::BottomClues));
    auto const left_rect (field_rects_.at (FieldType::LeftClues));
    auto const right_rect (field_rects_.at (FieldType::RightClues));
    auto const puzzle_rect (field_rects_.at (FieldType::Puzzle));

    nonogram_.fillDataLocks (true);
    nonogram_.fillClueLocks (true);

    {
      showMessageBox
        ( "<b>Nonogram</b> or <b>Paint by Numbers</b> is a logic puzzle game, "
          "in which squares in a field must be filled in or left blank "
          "according to the numbers at the side of the grid to reveal a "
          "hidden picture.<br><br>"
          "This tutorial will introduce the game mechanics and show you how "
          "to solve this very simple first puzzle!"
        , position
        );

      highlights_ = {field_rects_.at (FieldType::Puzzle)};

      update();
      QCoreApplication::processEvents (QEventLoop::AllEvents);
    }

    {
      showMessageBox
        ( "The highlighted field is the puzzle field, where you have to fill "
          "in the right squares to reveal the hidden picture."
        , position
        );

      highlights_ = { field_rects_.at (FieldType::LeftClues)
                    , field_rects_.at (FieldType::RightClues)
                    };

      update();
      QCoreApplication::processEvents (QEventLoop::AllEvents);
    }

    {
      showMessageBox
        ( "The highlighted fields are the clues for the rows of the picture, "
          "which tell you how many segments of filled-in squares there are "
          "in each row. If there are multiple segments in a row, they must be "
          "separated by an empty square.\n\n"
          "The left and right clues are identical and are only shown on both "
          "sides to ensure that at least one of them is visible in case "
          "you're solving a very large puzzle."
        , position
        );
      highlights_ = { field_rects_.at (FieldType::TopClues)
                    , field_rects_.at (FieldType::BottomClues)
                    };
      update();
      QCoreApplication::processEvents (QEventLoop::AllEvents);
    }

    {
      showMessageBox
        ( "These are the clues for the columns of the picture. The same rules "
          "apply for these."
        , position
        );

      highlights_ = { { top_rect.left() + 2 * slot_size
                      , top_rect.top()
                      , slot_size
                      , top_rect.height()
                      }
                    , { bottom_rect.left() + 2 * slot_size
                      , bottom_rect.top()
                      , slot_size
                      , bottom_rect.height()
                     }
                    };
      update();
      QCoreApplication::processEvents (QEventLoop::AllEvents);
    }

    {
      showMessageBox
        ( "Now please take a look at the middle column of the vertical clues. "
          "It says that the hidden picture contains two segments of two "
          "filled-in squares."
        , position
        );


      highlights_ = { { puzzle_rect.left() + 2 * slot_size
                      , puzzle_rect.top()
                      , slot_size
                      , 2 * slot_size
                      }
                    , { puzzle_rect.left() + 2 * slot_size
                      , puzzle_rect.top() + 3 * slot_size
                      , slot_size
                      , 2 * slot_size
                      }
                    };
      update();
      QCoreApplication::processEvents (QEventLoop::AllEvents);
    }

    {
      showMessageBox
        ( "Since the puzzle has five rows, there is only one possible way "
          "that the two segments of two squares can be placed in this "
          "column.\n\n"
          "Please fill in the highlighted squares by clicking on each square "
          "or by clicking and dragging the mouse over them in one go."
        , position
        );

      data::grid::Cells const slots_to_fill
        { {data::grid::Column {2}, data::grid::Row {0}}
        , {data::grid::Column {2}, data::grid::Row {1}}
        , {data::grid::Column {2}, data::grid::Row {3}}
        , {data::grid::Column {2}, data::grid::Row {4}}
        };
      waitUntilCellsAreFilled (slots_to_fill, data::Answer::Datum::Filled);
      nonogram_.lockData (slots_to_fill, true);
    }

    {
      highlights_ = { { left_rect.left() + slot_size
                      , left_rect.top()
                      , slot_size
                      , slot_size
                      }
                    , { right_rect.left()
                      , right_rect.top()
                      , slot_size
                      , slot_size
                     }
                    , { left_rect.left() + slot_size
                      , left_rect.top() + 4 * slot_size
                      , slot_size
                      , slot_size
                      }
                    , { right_rect.left()
                      , right_rect.top() + 4 * slot_size
                      , slot_size
                      , slot_size
                      }
                    };
      update();
      QCoreApplication::processEvents (QEventLoop::AllEvents);

      showMessageBox
        ( "Please take a look at the top and bottom rows of the horizontal "
          "clues now. It seems that only one square in these two rows can be "
          "filled. We happen to have just filled out one square in each of "
          "the two rows.\n\n"
        , position
        );
    }

    {
      highlights_ = { { puzzle_rect.left()
                      , puzzle_rect.top()
                      , 2 * slot_size
                      , slot_size
                      }
                    , { puzzle_rect.left() + 3 * slot_size
                      , puzzle_rect.top()
                      , 2 * slot_size
                      , slot_size
                      }
                    , { puzzle_rect.left()
                      , puzzle_rect.top() + 4 * slot_size
                      , 2 * slot_size
                      , slot_size
                      }
                    , { puzzle_rect.left() + 3 * slot_size
                      , puzzle_rect.top() + 4 * slot_size
                      , 2 * slot_size
                      , slot_size
                      }
                    };
      update();
      QCoreApplication::processEvents (QEventLoop::AllEvents);

      showMessageBox
        ( "Since the only required square is already filled, we can conclude "
          "that the rest of the squares in the rows must be empty.\n\n"
          "In order to make this information more apparent, please choose the "
          "Cross tool on the right of the Fill tool and cross out the "
          "highlighted squares."
        , position
        );

      data::grid::Cells const slots_to_fill
        { {data::grid::Column {0}, data::grid::Row {0}}
        , {data::grid::Column {1}, data::grid::Row {0}}
        , {data::grid::Column {3}, data::grid::Row {0}}
        , {data::grid::Column {4}, data::grid::Row {0}}
        , {data::grid::Column {0}, data::grid::Row {4}}
        , {data::grid::Column {1}, data::grid::Row {4}}
        , {data::grid::Column {3}, data::grid::Row {4}}
        , {data::grid::Column {4}, data::grid::Row {4}}
        };
      waitUntilCellsAreFilled (slots_to_fill, data::Answer::Datum::Crossed);
      nonogram_.lockData (slots_to_fill, true);
    }

    {
      highlights_ = { { top_rect.left() + slot_size
                      , top_rect.top() + slot_size
                      , slot_size
                      , slot_size
                      }
                    , { bottom_rect.left() + slot_size
                      , bottom_rect.top()
                      , slot_size
                      , slot_size
                     }
                    , { top_rect.left() + 3 * slot_size
                      , top_rect.top() + slot_size
                      , slot_size
                      , slot_size
                      }
                    , { bottom_rect.left() + 3 * slot_size
                      , bottom_rect.top()
                      , slot_size
                      , slot_size
                      }
                    };
      update();
      QCoreApplication::processEvents (QEventLoop::AllEvents);

      showMessageBox
        ( "Crossing out squares that are empty in the picture is not required "
          "to solve the puzzle, but it can help you see which squares in a "
          "row or column you have already ruled out.\n\n"
          "If you look at the highlighted clues, it should be obvious now "
          "that the segment of three filled-in squares can only be in the "
          "three empty squares that are left."
        , position
        );
    }

    {
      highlights_ = { { puzzle_rect.left() + slot_size
                      , puzzle_rect.top() + slot_size
                      , slot_size
                      , 3 * slot_size
                      }
                    , { puzzle_rect.left() + 3 * slot_size
                      , puzzle_rect.top() + slot_size
                      , slot_size
                      , 3 * slot_size
                      }
                    };
      update();
      QCoreApplication::processEvents (QEventLoop::AllEvents);

      showMessageBox
        ( "Please switch to the Fill tool and fill in the highlighted squares."
        , position
        );

      data::grid::Cells const slots_to_fill
        { {data::grid::Column {1}, data::grid::Row {1}}
        , {data::grid::Column {1}, data::grid::Row {2}}
        , {data::grid::Column {1}, data::grid::Row {3}}
        , {data::grid::Column {3}, data::grid::Row {1}}
        , {data::grid::Column {3}, data::grid::Row {2}}
        , {data::grid::Column {3}, data::grid::Row {3}}
        };
      waitUntilCellsAreFilled (slots_to_fill, data::Answer::Datum::Filled);
    }

    {
      highlights_.clear();
      update();
      QCoreApplication::processEvents (QEventLoop::AllEvents);

      showMessageBox
        ( "You should now know enough to complete the puzzle!<br><br>"
          "Good luck!"
        , position
        );

      nonogram_.fillDataLocks (false);
      nonogram_.fillClueLocks (false);
      update();
    }
  }
}
