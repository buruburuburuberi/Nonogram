#pragma once

#include <nonogram/data/Nonogram.hpp>
#include <nonogram/file/Puzzles.hpp>
#include <nonogram/gui/LevelSelection.hpp>
#include <nonogram/gui/PlayField.hpp>
#include <nonogram/gui/StartMenu.hpp>
#include <nonogram/util/unique_qt_ptr.hpp>

#include <QtCore/QSize>
#include <QtGui/QCloseEvent>
#include <QtGui/QColor>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QUndoStack>

#include <map>

namespace nonogram::gui
{
  class MainWindow : public QMainWindow
  {
    Q_OBJECT

    public:
      MainWindow();

    protected:
      void closeEvent (QCloseEvent*);

    private slots:
      void showLevel (data::Nonogram::ID);
      void showTutorial();

    private:
      void writeOutCurrentAnswer();
      void reset (bool solved);

      QSize const icon_size_;
      QColor const bg_color_;
      QColor const fg_color_;
      file::Puzzles puzzles_;
      StartMenu start_menu_;
      QUndoStack undo_stack_;
      data::Nonogram current_nonogram_;
      util::unique_qt_ptr<LevelSelection> level_selection_;
      QToolBar* level_selection_toolbar_;
      QToolBar* tools_toolbar_;
      util::unique_qt_ptr<QButtonGroup> tools_group_;
      util::unique_qt_ptr<QScrollArea> scroll_area_;
      util::unique_qt_ptr<PlayField> play_field_;
      util::unique_qt_ptr<QToolButton> check_button_;
      util::unique_qt_ptr<QToolButton> lock_button_;
      util::unique_qt_ptr<QToolButton> unlock_button_;
      util::unique_qt_ptr<QToolButton> reset_button_;
      util::unique_qt_ptr<QToolButton> fill_button_;
      util::unique_qt_ptr<QToolButton> cross_button_;
      util::unique_qt_ptr<QToolButton> fill_mark_button_;
      util::unique_qt_ptr<QToolButton> cross_mark_button_;
      util::unique_qt_ptr<QToolButton> redo_button_;
      util::unique_qt_ptr<QToolButton> undo_button_;
      util::unique_qt_ptr<QToolButton> solve_button_;
      util::unique_qt_ptr<QToolButton> controls_button_;
  };
}
