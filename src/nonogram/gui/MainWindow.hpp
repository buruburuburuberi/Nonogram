#pragma once

#include <nonogram/data/Nonogram.hpp>
#include <nonogram/gui/PlayField.hpp>
#include <nonogram/util/unique_qt_ptr.hpp>

#include <QtGui/QStandardItemModel>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QMainWindow>
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
        MainWindow (std::map<std::string, data::Nonogram>);

      private:
        QSize const icon_size_;
        QColor const bg_color_;
        QColor const fg_color_;
        QUndoStack undo_stack_;
        std::map<std::string, data::Nonogram> nonograms_;
        QStandardItemModel nonogram_model_;
        util::unique_qt_ptr<QComboBox> nonogram_list_;
        QToolBar* tools_toolbar_;
        util::unique_qt_ptr<QButtonGroup> tools_group_;
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
    };
  }
}
