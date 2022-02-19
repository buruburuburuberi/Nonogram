#pragma once

#include <nonogram/data/Answer.hpp>

#include <QtCore/QRect>
#include <QtCore/QSize>
#include <QtGui/QColor>
#include <QtGui/QIcon>
#include <QtGui/QPainter>

namespace nonogram::gui
{
  void drawBackground (QPainter&, QRect, QColor);
  void drawDatum (QPainter&, QRect, data::Answer::Datum, QColor, bool solved);
  void drawCross (QPainter&, QRect, QColor);

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
