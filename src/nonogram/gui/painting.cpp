#include <nonogram/gui/painting.hpp>

#include <nonogram/util/CenteredRect.hpp>
#include <nonogram/util/ScopedQPainterState.hpp>

#include <QtCore/QRectF>
#include <QtGui/QBrush>
#include <QtGui/QPen>
#include <QtGui/QPixmap>

namespace nonogram::gui
{
  void drawBackground (QPainter& painter, QRect rect, QColor color)
  {
    util::ScopedQPainterState const state (painter);

    painter.fillRect (rect.adjusted (1, 1, -1, -1), color);
  }

  void drawFill ( QPainter& painter
                , QRect background_rect
                , QColor color
                , bool solved
                )
  {
    util::ScopedQPainterState const state (painter);

    auto const rect
      ( solved
      ? background_rect
      : util::CenteredRect
          ( background_rect.center()
          , QSize (background_rect.width() - 6, background_rect.height() - 6)
          )
      );

    painter.fillRect (rect, color);
  }

  void drawCross ( QPainter& painter
                 , QRect background_rect
                 , QColor color
                 )
  {
    util::ScopedQPainterState const state (painter);

    auto pen (painter.pen());
    pen.setWidth (2);
    pen.setColor (color);
    painter.setPen (pen);

    std::size_t const side (background_rect.width() - 8);
    QRect const cross_rect
      ( util::CenteredRect
          ( background_rect.center()
          , QSize (side, side)
          )
      );

    painter.drawLine (cross_rect.topLeft(), cross_rect.bottomRight());
    painter.drawLine (cross_rect.bottomLeft(), cross_rect.topRight());
  }

  void drawFillMark ( QPainter& painter
                    , QRect background_rect
                    , QColor color
                    )
  {
    util::ScopedQPainterState const state (painter);

    painter.setBrush (color);

    std::size_t const side (background_rect.width() / 3);
    painter.drawEllipse
      (util::CenteredRect (background_rect.center(), QSize (side, side)));
  }

  void drawCrossMark ( QPainter& painter
                     , QRect background_rect
                     , QColor color
                     )
  {
    util::ScopedQPainterState const state (painter);

    auto pen (painter.pen());
    pen.setWidth (2);
    pen.setColor (color);
    painter.setPen (pen);

    auto const side (background_rect.width() - 8);
    QRect const cross_mark_rect
      (util::CenteredRect (background_rect.center(), QSize (side, side)));

    painter.drawLine ( cross_mark_rect.bottomLeft()
                     , cross_mark_rect.topRight()
                     );
  }

  QIcon createIcon ( data::Answer::Datum datum
                   , QSize size
                   , QColor bg_color
                   , QColor fg_color
                   )
  {
    QPixmap pixmap (size);
    QPainter painter (&pixmap);
    pixmap.fill (Qt::black);

    drawBackground (painter, pixmap.rect(), bg_color);

    switch (datum)
    {
      case data::Answer::Datum::Empty:
      {
        break;
      }
      case data::Answer::Datum::Filled:
      {
        drawFill (painter, pixmap.rect(), fg_color, false);
        break;
      }
      case data::Answer::Datum::Crossed:
      {
        drawCross (painter, pixmap.rect(), fg_color);
        break;
      }
      case data::Answer::Datum::FillMark:
      {
        drawFillMark (painter, pixmap.rect(), fg_color);
        break;
      }
      case data::Answer::Datum::CrossMark:
      {
        drawCrossMark (painter, pixmap.rect(), fg_color);
        break;
      }
      default:
      {
        throw std::invalid_argument ("Unknown datum type.");
      }
    }

    return pixmap;
  }

  QIcon createCheckIcon (QSize size, QColor bg_color, QColor fg_color)
  {
    QPixmap pixmap (size);
    QPainter painter (&pixmap);
    pixmap.fill (Qt::black);

    drawBackground (painter, pixmap.rect(), bg_color);

    QFont font;
    font.setPixelSize (20);
    font.setBold (true);
    painter.setFont (font);

    painter.drawText (pixmap.rect(), Qt::AlignCenter, "C");

    return pixmap;
  }

  QIcon createLockIcon (QSize size, QColor bg_color, QColor fg_color)
  {
    QPixmap pixmap (size);
    QPainter painter (&pixmap);
    pixmap.fill (Qt::black);

    drawBackground (painter, pixmap.rect(), bg_color);

    auto pen (painter.pen());
    pen.setWidth (2);
    painter.setPen (pen);

    auto const width (size.width());
    auto const height (size.height());

    QRectF const arc_rect
      (QPoint (width * 0.3, height * 0.2), QPoint (width * 0.7, height * 0.8));
    int const startAngle = 0;
    int const spanAngle = 180 * 16;

    painter.drawArc (arc_rect, startAngle, spanAngle);

    painter.fillRect
      ( QRect ( QPoint (width * 0.2, height * 0.45)
              , QPoint (width * 0.8, height * 0.85)
              )
      , fg_color
      );

    return pixmap;
  }

  QIcon createUnlockIcon (QSize size, QColor bg_color, QColor fg_color)
  {
    QPixmap pixmap (size);
    QPainter painter (&pixmap);
    pixmap.fill (Qt::black);

    drawBackground (painter, pixmap.rect(), bg_color);

    auto pen (painter.pen());
    pen.setWidth (2);
    painter.setPen (pen);

    auto const width (size.width());
    auto const height (size.height());

    QRectF const arc_rect
      (QPoint (width * 0.3, height * 0.2), QPoint (width * 0.7, height * 0.8));
    int const startAngle = 180 * 16;
    int const spanAngle = -120 * 16;

    painter.drawArc (arc_rect, startAngle, spanAngle);

    painter.fillRect
      ( QRect ( QPoint (width * 0.2, height * 0.45)
              , QPoint (width * 0.8, height * 0.85)
              )
      , fg_color
      );

    return pixmap;
  }

  QIcon createUndoIcon (QSize size, QColor bg_color, QColor fg_color)
  {
    QPixmap pixmap (size);
    QPainter painter (&pixmap);
    pixmap.fill (Qt::black);

    drawBackground (painter, pixmap.rect(), bg_color);

    painter.setBrush (fg_color);

    auto const width (size.width());
    auto const height (size.width());
    std::array<QPointF, 3> const points
      { QPointF (width * 0.8, height * 0.2)
      , QPointF (width * 0.8, height * 0.8)
      , QPointF (width * 0.2, height / 2)
      };

    painter.drawPolygon (points.data(), points.size());

    return pixmap;
  }

  QIcon createRedoIcon (QSize size, QColor bg_color, QColor fg_color)
  {
    QPixmap pixmap (size);
    QPainter painter (&pixmap);
    pixmap.fill (Qt::black);

    drawBackground (painter, pixmap.rect(), bg_color);

    painter.setBrush (fg_color);

    auto const width (size.width());
    auto const height (size.width());
    std::array<QPointF, 3> const points
      { QPointF (width * 0.2, height * 0.2)
      , QPointF (width * 0.8, height / 2)
      , QPointF (width * 0.2, height * 0.8)
      };

    painter.drawPolygon (points.data(), points.size());

    return pixmap;
  }

  QIcon createResetIcon (QSize size, QColor bg_color, QColor fg_color)
  {
    QPixmap pixmap (size);
    QPainter painter (&pixmap);
    pixmap.fill (Qt::black);

    drawBackground (painter, pixmap.rect(), bg_color);

    return pixmap;
  }

  QIcon createSolveIcon (QSize size, QColor bg_color, QColor fg_color)
  {
    QPixmap pixmap (size);
    QPainter painter (&pixmap);
    pixmap.fill (Qt::black);

    drawBackground (painter, pixmap.rect(), bg_color);

    painter.setBrush (fg_color);

    auto const width (size.width());
    auto const height (size.width());
    std::array<QPointF, 4> const points
      { QPointF (width * 0.2, height * 0.4)
      , QPointF (width * 0.4, height * 0.8)
      , QPointF (width * 0.8, height * 0.2)
      , QPointF (width * 0.4, height * 0.5)
      };

    painter.drawPolygon (points.data(), points.size());

    return pixmap;
  }

  QIcon createControlsIcon (QSize size, QColor bg_color, QColor fg_color)
  {
    QPixmap pixmap (size);
    QPainter painter (&pixmap);
    pixmap.fill (Qt::black);

    drawBackground (painter, pixmap.rect(), bg_color);

    QFont font;
    font.setPixelSize (20);
    font.setBold (true);
    painter.setFont (font);

    painter.drawText (pixmap.rect(), Qt::AlignCenter, "?");

    return pixmap;
  }
}
