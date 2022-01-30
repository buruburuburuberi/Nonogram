#include <nonogram/gui/PlayField.hpp>

#include <QtGui/QPainter>

namespace nonogram
{
  namespace gui
  {
    PlayField::PlayField (QWidget *parent)
    : QOpenGLWidget (parent)
    , fill_mode_ (data::Answer::Datum::Filled)
    {
      setAutoFillBackground (true);
    }

    PlayField::NonogramData::NonogramData ( data::Nonogram nonogram
                                          , QSize window_size
                                          )
    : font_size (18)
    , slot_size (30)
    , data (nonogram)
    {
      update (window_size);
    }

    void PlayField::NonogramData::update (QSize window_size)
    {
      std::size_t const border (2);
      QSize const size_of_left_clues
        ( data.columns_of_clues (data::Solution::ClueType::Left).value * slot_size
        , data.rows_of_clues (data::Solution::ClueType::Left).value * slot_size
        );
      QSize const size_of_top_clues
        ( data.columns_of_clues (data::Solution::ClueType::Top).value * slot_size
        , data.rows_of_clues (data::Solution::ClueType::Top).value * slot_size
        );
      QSize const size_of_right_clues
        ( data.columns_of_clues (data::Solution::ClueType::Right).value * slot_size
        , data.rows_of_clues (data::Solution::ClueType::Right).value * slot_size
        );
      QSize const size_of_bottom_clues
        ( data.columns_of_clues (data::Solution::ClueType::Bottom).value * slot_size
        , data.rows_of_clues (data::Solution::ClueType::Bottom).value * slot_size
        );
      QSize const puzzle_size ( data.columns_of_data().value * slot_size
                              , data.rows_of_data().value * slot_size
                              );

      clues_rects[data::Solution::ClueType::Left]
        = QRect ( QPoint (0, size_of_top_clues.height() + border)
                , size_of_left_clues
                );
      clues_rects[data::Solution::ClueType::Top]
        = QRect ( QPoint (size_of_left_clues.width() + border, 0)
                , size_of_top_clues
                );
      puzzle_rect =
          QRect ( QPoint ( clues_rects.at (data::Solution::ClueType::Top).left()
                         , clues_rects.at (data::Solution::ClueType::Left).top()
                         )
                , puzzle_size
                );
      clues_rects[data::Solution::ClueType::Right]
        = QRect ( QPoint ( puzzle_rect.x() + puzzle_rect.width() + border
                         , clues_rects.at (data::Solution::ClueType::Left).top()
                         )
                , size_of_right_clues
                );
      clues_rects[data::Solution::ClueType::Bottom]
        = QRect ( QPoint ( clues_rects.at (data::Solution::ClueType::Top).left()
                         , puzzle_rect.y() + puzzle_rect.height() + border
                         )
                , size_of_bottom_clues
                );
      field_rect =
          QRect ( QPoint ( clues_rects.at (data::Solution::ClueType::Left).x()
                         , clues_rects.at (data::Solution::ClueType::Top).y()
                         )
                , QPoint ( clues_rects.at (data::Solution::ClueType::Right).right()
                         , clues_rects.at (data::Solution::ClueType::Bottom).bottom()
                         )
                );

      QPoint const offset ( (window_size.width() - field_rect.width()) / 2.0f
                          , (window_size.height() - field_rect.height()) / 2.0f
                          );
      puzzle_rect.translate (offset);
      clues_rects.at (data::Solution::ClueType::Left).translate (offset);
      clues_rects.at (data::Solution::ClueType::Right).translate (offset);
      clues_rects.at (data::Solution::ClueType::Top).translate (offset);
      clues_rects.at (data::Solution::ClueType::Bottom).translate (offset);
      field_rect.translate (offset);
    }

    void PlayField::setFillMode (data::Answer::Datum mode)
    {
      fill_mode_ = mode;
    }

    void PlayField::setNonogram (data::Nonogram nonogram)
    {
      nonogram_ = NonogramData (std::move (nonogram), size());

      setMinimumSize (nonogram_->field_rect.size() + QSize (50, 50));

      update();
    }

    QPoint PlayField::clueCenter ( QRect clues_rect
                                 , data::Column column
                                 , data::Row row
                                 ) const
    {
      if (!nonogram_)
      {
        return QPoint();
      }

      auto const half_slot_size (nonogram_->slot_size / 2.f);
      return QPoint
        ( clues_rect.x() + column.value * nonogram_->slot_size + half_slot_size
        , clues_rect.y() + row.value * nonogram_->slot_size + half_slot_size
        );
    }

    void PlayField::drawClue ( QPainter& painter
                             , data::Solution::ClueType type
                             , data::Column column
                             , data::Row row
                             )
    {
      if (!nonogram_)
      {
        return;
      }

      auto const clue_center
        (clueCenter (nonogram_->clues_rects.at (type), column, row));

      auto const background_size (nonogram_->slot_size - 2);
      QRect background_rect (0, 0, background_size, background_size);
      background_rect.moveCenter (clue_center);

      painter.fillRect (background_rect, Qt::yellow);

      auto const slot_size
        ( pressed_clue_
        && pressed_clue_->column.value == column.value
        && pressed_clue_->row.value == row.value
        ? nonogram_->slot_size - 4
        : nonogram_->slot_size
        );

      auto const clue (nonogram_->data.clue (type, column, row));

      if (clue > 0)
      {
        painter.setPen (Qt::black);
        auto const text_size (slot_size);
        QFont font;
        font.setPixelSize (nonogram_->font_size);
        painter.setFont (font);
        QRect text_rect (0, 0, text_size, text_size);
        text_rect.moveCenter (clue_center);
        painter.drawText (text_rect, Qt::AlignCenter, QString::number (clue));

        if (nonogram_->data.is_crossed (type, column, row))
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
      if (!nonogram_)
      {
        return;
      }

      for ( data::Column column {0}
          ; column.value < nonogram_->data.columns_of_clues (type).value
          ; ++column.value
          )
      {
        for ( data::Row row {0}
            ; row.value < nonogram_->data.rows_of_clues (type).value
            ; ++row.value
            )
        {
          drawClue (painter, type, column, row);
        }
      }
    }

    data::Slot PlayField::fromPosition (QRect rect, QPoint position) const
    {
      return { data::Column {(position.x() - rect.x()) / nonogram_->slot_size}
             , data::Row {(position.y() - rect.y()) / nonogram_->slot_size}
             };
    }

    QPoint PlayField::slotCenter (data::Column column, data::Row row) const
    {
      if (!nonogram_)
      {
        return QPoint();
      }

      auto const half_slot_size (nonogram_->slot_size / 2.f);
      return QPoint
        ( nonogram_->puzzle_rect.x() + column.value * nonogram_->slot_size + half_slot_size
        , nonogram_->puzzle_rect.y() + row.value * nonogram_->slot_size + half_slot_size
        );
    }

    void PlayField::drawSlot ( QPainter& painter
                             , data::Column column
                             , data::Row row
                             , data::Answer::Datum datum
                             )
    {
      if (!nonogram_)
      {
        return;
      }

      auto const slot_center (slotCenter (column, row));

      auto const background_size (nonogram_->slot_size - 2);
      QRect background_rect (0, 0, background_size, background_size);
      background_rect.moveCenter (slot_center);

      painter.fillRect (background_rect, Qt::yellow);

      painter.setBrush (Qt::black);
      auto pen (painter.pen());
      pen.setWidth (2);
      pen.setColor (Qt::black);
      painter.setPen (pen);

      auto const slot_size
        ( pressed_slot_
        && pressed_slot_->column.value == column.value
        && pressed_slot_->row.value == row.value
        ? nonogram_->slot_size - 4
        : nonogram_->slot_size
        );

      switch (datum)
      {
        case data::Answer::Datum::Empty:
        {
          break;
        }
        case data::Answer::Datum::Filled:
        {
          auto const point_size (slot_size - 6);
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
      if (!nonogram_)
      {
        return;
      }

      for ( data::Column column {0}
          ; column.value < nonogram_->data.columns_of_data().value
          ; ++column.value
          )
      {
        for ( data::Row row {0}
            ; row.value < nonogram_->data.rows_of_data().value
            ; ++row.value
            )
        {
          drawSlot (painter, column, row, nonogram_->data.at (column, row));
        }
      }
    }

    bool PlayField::fillSlot (QPoint position)
    {
      if (!nonogram_)
      {
        return false;
      }

      if (nonogram_->puzzle_rect.contains (position))
      {
        auto const slot (fromPosition (nonogram_->puzzle_rect, position));

        nonogram_->data.set (slot.column, slot.row, fill_mode_);
        pressed_slot_ = {slot.column, slot.row};

        update();

        return true;
      }

      return false;
    }

    bool PlayField::crossClue (QPoint position, bool first_press)
    {
      if (!nonogram_)
      {
        return false;
      }

      for (auto const& rect : nonogram_->clues_rects)
      {
        if (rect.second.contains (position))
        {
          auto const slot (fromPosition (rect.second, position));

          if (first_press)
          {
            current_clue_state_ =
                !nonogram_->data.is_crossed (rect.first, slot.column, slot.row);
          }

          nonogram_->data.set_crossed ( rect.first
                                      , slot.column
                                      , slot.row
                                      , current_clue_state_.value()
                                      );
          pressed_clue_ = {slot.column, slot.row};

          update();

          return true;
        }
      }

      return false;
    }

    void PlayField::mouseMoveEvent (QMouseEvent* event)
    {
      if (!nonogram_)
      {
        return;
      }

      if (event->buttons() & Qt::LeftButton)
      {
        if (fillSlot (event->pos()))
        {
          return;
        }
        else if (crossClue (event->pos(), false))
        {
          return;
        }
      }
    }

    void PlayField::mousePressEvent (QMouseEvent* event)
    {
      if (!nonogram_)
      {
        return;
      }

      if (event->buttons() & Qt::LeftButton)
      {
        if (fillSlot (event->pos()))
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
      if (!nonogram_)
      {
        return;
      }

      if (pressed_slot_ || pressed_clue_)
      {
        pressed_slot_.reset();
        pressed_clue_.reset();
        current_clue_state_.reset();

        update();
      }
    }

    void PlayField::paintGL()
    {
      QPainter painter (this);
      painter.fillRect (rect(), palette().color (QPalette::Window));

      if (!nonogram_)
      {
        return;
      }

      painter.fillRect (rect(), Qt::black);

      drawClues (painter, data::Solution::ClueType::Left);
      drawClues (painter, data::Solution::ClueType::Top);
      drawClues (painter, data::Solution::ClueType::Right);
      drawClues (painter, data::Solution::ClueType::Bottom);
      drawPuzzle (painter);
    }

    void PlayField::resizeGL (int width, int height)
    {
      if (nonogram_)
      {
        nonogram_->update (QSize (width, height));

        update();
      }
    }
  };
}
