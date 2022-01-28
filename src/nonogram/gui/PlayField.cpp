#include <nonogram/gui/PlayField.hpp>

#include <QtGui/QPainter>

namespace nonogram
{
  namespace gui
  {
    PlayField::PlayField (QWidget *parent)
    : QOpenGLWidget (parent)
    , slot_size_ (30)
    , fill_size_ (28)
    {
      setAutoFillBackground (true);
      setMinimumWidth (800);
      setMinimumHeight (800);
    }

    void PlayField::setNonogram (data::Nonogram nonogram)
    {
      nonogram_ = std::move (nonogram);

      update();
    }

    QRect PlayField::slotRect (int column, int row) const
    {
      if (!nonogram_)
      {
        return QRect();
      }

      auto const field_width (nonogram_->columns() * slot_size_);
      auto const field_height (nonogram_->rows() * slot_size_);
      auto const field_offset_x ((width() - field_width) / 2.0f);
      auto const field_offset_y ((height() - field_height) / 2.0f);
      auto const slot_offset ((slot_size_ - fill_size_) / 2);
      return QRect ( field_offset_x + column * slot_size_ + slot_offset
                   , field_offset_y + row * slot_size_ + slot_offset
                   , fill_size_
                   , fill_size_
                   );
    }

    void PlayField::drawSlot ( QPainter* painter
                             , int column
                             , int row
                             , data::Nonogram::Datum
                             )
    {
      painter->fillRect (slotRect (column, row), Qt::yellow);
    }

    void PlayField::paintEvent (QPaintEvent* event)
    {
      QPainter painter (this);

      if (!nonogram_)
      {
        painter.fillRect (rect(), Qt::lightGray);
        return;
      }

      painter.fillRect (rect(), Qt::black);

      for (unsigned int column (0); column < nonogram_->columns(); ++column)
      {
        for (unsigned int row (0); row < nonogram_->rows(); ++row)
        {
          drawSlot (&painter, column, row, nonogram_->at (column, row));
        }
      }
    }
  };
}
