#include <nonogram/gui/PlayField.hpp>

#include <QtGui/QPainter>

namespace nonogram
{
  namespace gui
  {
    PlayField::PlayField (QWidget *parent)
    : QOpenGLWidget (parent)
    {
      setAutoFillBackground (true);
    }

    PlayField::NonogramData::NonogramData ( data::Nonogram nonogram
                                          , QSize window_size
                                          )
    : slot_size (30)
    , data (nonogram)
    {
      update (window_size);
    }

    void PlayField::NonogramData::update (QSize window_size)
    {
      std::size_t const border (2);
      QSize const clues_size (data.size_of_clues() * slot_size);
      QSize const puzzle_size ( data.columns().value * slot_size
                              , data.rows().value * slot_size
                              );
      QSize const clues_size_columns (puzzle_size.width(), clues_size.height());
      QSize const clues_size_rows (clues_size.width(), puzzle_size.height());
      left_clues_rect = QRect ( QPoint (0, clues_size.height() + border)
                              , clues_size_rows
                              );
      top_clues_rect = QRect ( QPoint (clues_size.width() + border, 0)
                             , clues_size_columns
                             );
      puzzle_rect = QRect ( QPoint ( top_clues_rect.left()
                                   , left_clues_rect.top()
                                   )
                          , puzzle_size
                          );
      right_clues_rect =
          QRect ( QPoint ( puzzle_rect.x() + puzzle_rect.width() + border
                         , left_clues_rect.top()
                         )
                , clues_size_rows
                );
      bottom_clues_rect =
          QRect ( QPoint ( top_clues_rect.left()
                         , puzzle_rect.y() + puzzle_rect.height() + border
                         )
                , clues_size_columns
                );
      field_rect =
          QRect ( QPoint (left_clues_rect.x(), top_clues_rect.y())
                , QPoint (right_clues_rect.right(), bottom_clues_rect.bottom())
                );

      QPoint const offset ( (window_size.width() - field_rect.width()) / 2.0f
                          , (window_size.height() - field_rect.height()) / 2.0f
                          );
      puzzle_rect.translate (offset);
      left_clues_rect.translate (offset);
      right_clues_rect.translate (offset);
      top_clues_rect.translate (offset);
      bottom_clues_rect.translate (offset);
      field_rect.translate (offset);
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
                             , QRect clue_rect
                             , data::Column column
                             , data::Row row
                             , std::optional<data::Solution::Clue> maybe_clue
                             )
    {
      if (!nonogram_)
      {
        return;
      }

      auto const clue_center (clueCenter (clue_rect, column, row));

      auto const background_size (nonogram_->slot_size - 2);
      QRect background_rect (0, 0, background_size, background_size);
      background_rect.moveCenter (clue_center);

      painter.fillRect (background_rect, Qt::yellow);

      if (maybe_clue)
      {
        painter.setPen (Qt::black);
        auto const text_size (nonogram_->slot_size - 6);
        QRect text_rect (0, 0, text_size, text_size);
        text_rect.moveCenter (clue_center);
        painter.drawText ( text_rect
                         , Qt::AlignCenter
                         , QString::number (maybe_clue.value())
                         );
      }
    }

    void PlayField::drawClues (QPainter& painter)
    {
      if (!nonogram_)
      {
        return;
      }

      auto maybe_get_clue
        ( [] (data::Solution::Clues const& clues, std::size_t index)
          {
            return index < clues.size()
                ? std::optional (clues.at (index))
                : std::nullopt;
          }
        );

      auto const size_of_clues (nonogram_->data.size_of_clues());
      for ( data::Column column {0}
          ; column.value < nonogram_->data.columns().value
          ; ++column.value
          )
      {
        auto const& clues (nonogram_->data.clues (column));
        for (data::Row row {0}; row.value < size_of_clues.height(); ++row.value)
        {
          drawClue ( painter
                   , nonogram_->top_clues_rect
                   , column
                   , data::Row {size_of_clues.height() - row.value - 1}
                   , maybe_get_clue (clues, clues.size() - row.value - 1)
                   );
          drawClue ( painter
                   , nonogram_->bottom_clues_rect
                   , column
                   , row
                   , maybe_get_clue (clues, row.value)
                   );
        }
      }

      for ( data::Row row {0}
          ; row.value < nonogram_->data.rows().value
          ; ++row.value
          )
      {
        auto const& clues (nonogram_->data.clues (row));
        for ( data::Column column {0}
            ; column.value < size_of_clues.width()
            ; ++column.value
            )
        {
          drawClue ( painter
                   , nonogram_->left_clues_rect
                   , data::Column {size_of_clues.width() - column.value - 1}
                   , row
                   , maybe_get_clue (clues, clues.size() - column.value - 1)
                   );
          drawClue ( painter
                   , nonogram_->right_clues_rect
                   , column
                   , row
                   , maybe_get_clue (clues, column.value)
                   );
        }
      }
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
                             , data::Nonogram::Datum datum
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

      switch (datum)
      {
        case data::Nonogram::Datum::Empty:
        {
          break;
        }
        case data::Nonogram::Datum::Filled:
        {
          auto const point_size (nonogram_->slot_size - 6);
          QRect point_rect (0, 0, point_size, point_size);
          point_rect.moveCenter (slot_center);

          painter.fillRect (point_rect, painter.pen().color());
          break;
        }
        case data::Nonogram::Datum::Crossed:
        {
          auto const cross_size (nonogram_->slot_size - 8);
          QRect cross_rect (0, 0, cross_size, cross_size);
          cross_rect.moveCenter (slot_center);

          painter.drawLine (cross_rect.topLeft(), cross_rect.bottomRight());
          painter.drawLine (cross_rect.bottomLeft(), cross_rect.topRight());
          break;
        }
        case data::Nonogram::Datum::FillMark:
        {
          auto const fill_mark_size ((nonogram_->slot_size - 8.0f) / 2.0f);
          QRect fill_mark_rect (0, 0, fill_mark_size, fill_mark_size);
          fill_mark_rect.moveCenter (slot_center);

          painter.drawEllipse (fill_mark_rect);
          break;
        }
        case data::Nonogram::Datum::CrossMark:
        {
          auto const cross_mark_size (nonogram_->slot_size - 8);
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
          ; column.value < nonogram_->data.columns().value
          ; ++column.value
          )
      {
        for ( data::Row row {0}
            ; row.value < nonogram_->data.rows().value
            ; ++row.value
            )
        {
          drawSlot (painter, column, row, nonogram_->data.at (column, row));
        }
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

      drawClues (painter);
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
