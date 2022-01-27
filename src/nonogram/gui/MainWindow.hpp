#pragma once

#include <nonogram/data/Nonogram.hpp>
#include <nonogram/gui/PlayField.hpp>
#include <nonogram/util/unique_qt_ptr.hpp>

#include <QtGui/QStandardItemModel>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QMainWindow>

#include <map>
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
        std::map<std::string, data::Nonogram> nonograms_;
        QStandardItemModel nonogram_model_;
        util::unique_qt_ptr<QComboBox> nonogram_list_;
        util::unique_qt_ptr<PlayField> play_field_;
    };
  }
}
