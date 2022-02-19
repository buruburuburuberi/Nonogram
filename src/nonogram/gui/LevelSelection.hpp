#pragma once

#include <nonogram/data/Nonogram.hpp>
#include <nonogram/file/Puzzles.hpp>
#include <nonogram/util/unique_qt_ptr.hpp>

#include <QtCore/QString>
#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QCommandLinkButton>
#include <QtWidgets/QMenu>
#include <QtWidgets/QWidget>

#include <map>

namespace nonogram::gui
{
  class LevelSelection : public QWidget
  {
    Q_OBJECT

    public:
      LevelSelection (file::Puzzles const&, QBoxLayout::Direction);

      void markAsSolved (data::Nonogram::ID);

    signals:
      void levelSelected (data::Nonogram::ID);

    private:
      void setPack (data::Nonogram::Pack);

      QIcon unsolved_icon_;
      QIcon solved_icon_;
      file::Puzzles const& puzzles_;
      std::map<data::Nonogram::ID, util::unique_qt_ptr<QAction>> puzzle_actions_;
      std::map<data::Nonogram::Pack, util::unique_qt_ptr<QMenu>> nonogram_menus_;
      std::map<data::Nonogram::Pack, util::unique_qt_ptr<QCommandLinkButton>>
        nonogram_selections_;
  };
}
