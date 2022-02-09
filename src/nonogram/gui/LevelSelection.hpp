#pragma once

#include <nonogram/file/Puzzles.hpp>
#include <nonogram/util/unique_qt_ptr.hpp>

#include <QtCore/QString>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QMenu>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

#include <map>

namespace nonogram
{
  namespace gui
  {
    class LevelSelection : public QWidget
    {
      Q_OBJECT

      public:
        enum class Direction
        { Horizontal
        , Vertical
        };

        LevelSelection (file::Puzzles const&, QBoxLayout::Direction);

        void setLevel (QString pack, QString puzzle);

      signals:
        void levelSelected (QString pack, QString puzzle);

      private:
        void setPack (QString pack);

        QString select_difficulty_text_;
        QString select_nonogram_text_;
        file::Puzzles const& puzzles_;
        util::unique_qt_ptr<QMenu> pack_menu_;
        util::unique_qt_ptr<QToolButton> pack_selection_;
        std::map<QString, util::unique_qt_ptr<QMenu>> nonogram_menus_;
        util::unique_qt_ptr<QToolButton> nonogram_selection_;
    };
  }
}
