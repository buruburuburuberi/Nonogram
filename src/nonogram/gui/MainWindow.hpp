#pragma once

#include <nonogram/data/Nonogram.hpp>
#include <nonogram/file/Puzzles.hpp>
#include <nonogram/gui/PlayField.hpp>
#include <nonogram/util/unique_qt_ptr.hpp>

#include <QtCore/QString>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QUndoStack>

#include <map>
#include <string>
#include <vector>

namespace nonogram
{
  namespace gui
  {
    class MainWindow : public QMainWindow
    {
      Q_OBJECT

      public:
        MainWindow();

      private:
        QString const select_difficulty_text_;
        QString const select_level_text_;
        file::Puzzles const puzzles_;
        QSize const icon_size_;
        QColor const bg_color_;
        QColor const fg_color_;
        QUndoStack undo_stack_;
        QStandardItemModel pack_model_;
        std::map<QString, QStandardItemModel> nonogram_models_;
        util::unique_qt_ptr<QComboBox> pack_list_;
        util::unique_qt_ptr<QComboBox> nonogram_list_;
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
    };
  }
}
