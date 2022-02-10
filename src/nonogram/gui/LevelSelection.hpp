#pragma once

#include <nonogram/file/Puzzles.hpp>
#include <nonogram/util/unique_qt_ptr.hpp>

#include <QtCore/QString>
#include <QtGui/QIcon>
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

        void setLevel (data::Nonogram::ID);
        void setSolved (data::Nonogram::ID);

      signals:
        void levelSelected (data::Nonogram::ID);

      private:
        void setPack (data::Nonogram::Pack);

        QIcon solved_icon_;
        QString select_difficulty_text_;
        QString select_nonogram_text_;
        file::Puzzles const& puzzles_;
        util::unique_qt_ptr<QMenu> pack_menu_;
        util::unique_qt_ptr<QToolButton> pack_selection_;
        std::map<data::Nonogram::Pack, util::unique_qt_ptr<QMenu>> nonogram_menus_;
        std::map<data::Nonogram::Pack, util::unique_qt_ptr<QAction>> pack_actions_;
        std::map<data::Nonogram::ID, util::unique_qt_ptr<QAction>> puzzle_actions_;
        util::unique_qt_ptr<QToolButton> nonogram_selection_;
    };
  }
}
