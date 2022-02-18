#pragma once

#include <nonogram/data/Answer.hpp>

#include <QtCore/QRect>
#include <QtCore/QSize>
#include <QtGui/QColor>
#include <QtGui/QIcon>
#include <QtGui/QPainter>

namespace nonogram
{
  namespace gui
  {
    void drawBackground (QPainter&, QRect, QColor);
    void drawFill ( QPainter&
                  , QRect background
                  , QColor
                  , bool solved
                  );
    void drawCross ( QPainter&
                   , QRect background
                   , QColor
                   );
    void drawFillMark ( QPainter&
                      , QRect background
                      , QColor
                      );
    void drawCrossMark ( QPainter&
                       , QRect background
                       , QColor
                       );

    QIcon createIcon (data::Answer::Datum, QSize, QColor bg, QColor fg);
    QIcon createCheckIcon (QSize, QColor bg, QColor fg);
    QIcon createLockIcon (QSize, QColor bg, QColor fg);
    QIcon createUnlockIcon (QSize, QColor bg, QColor fg);
    QIcon createUndoIcon (QSize, QColor bg, QColor fg);
    QIcon createRedoIcon (QSize, QColor bg, QColor fg);
    QIcon createResetIcon (QSize, QColor bg, QColor fg);
    QIcon createSolveIcon (QSize, QColor bg, QColor fg);
    QIcon createControlsIcon (QSize, QColor bg, QColor fg);
  }
}
