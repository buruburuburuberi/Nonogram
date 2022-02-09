#pragma once

#include <nonogram/file/Puzzles.hpp>
#include <nonogram/gui/LevelSelection.hpp>
#include <nonogram/util/unique_qt_ptr.hpp>

#include <QtWidgets/QCommandLinkButton>
#include <QtWidgets/QDialog>
#include <QtWidgets/QWidget>

namespace nonogram
{
  namespace gui
  {
    class StartMenu : public QDialog
    {
      Q_OBJECT

      public:
        StartMenu (file::Puzzles const&, QWidget* parent);

      signals:
        void nonogramSelected (QString pack, QString puzzle);

      private:
        util::unique_qt_ptr<QCommandLinkButton> continue_button_;
        util::unique_qt_ptr<LevelSelection> level_selection_;
        util::unique_qt_ptr<QCommandLinkButton> quit_button_;
    };
  }
}
