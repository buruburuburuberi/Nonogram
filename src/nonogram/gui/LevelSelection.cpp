#include <nonogram/gui/LevelSelection.hpp>

#include <QtCore/QList>
#include <QtWidgets/QAction>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QStyle>

namespace nonogram
{
  namespace gui
  {
    LevelSelection::LevelSelection ( file::Puzzles const& puzzles
                                   , QBoxLayout::Direction direction
                                   )
    : QWidget()
    , unsolved_icon_ (style()->standardIcon (QStyle::SP_CommandLink))
    , solved_icon_ (style()->standardIcon (QStyle::SP_DialogApplyButton))
    , nonogram_selections_()
    , puzzles_ (puzzles)
    {
      auto add_nonogram
      ( [&] (QMenu* menu, data::Nonogram::ID id, bool solved)
        {
          util::unique_qt_ptr<QAction> action (id.puzzle.name);
          if (solved)
          {
            action->setIcon (solved_icon_);
          }

          connect ( action.get()
                  , &QAction::triggered
                  , this
                  , [this, id] { emit levelSelected (id); }
                  );


          menu->addAction (action.release());

          puzzle_actions_.emplace
            ( std::piecewise_construct
            , std::forward_as_tuple (id)
            , std::forward_as_tuple (std::move (action))
            );
        }
      );

      util::unique_qt_ptr<QLabel> selection_label ("<b>Select Nonogram:</b>");
      selection_label->setTextFormat (Qt::RichText);

      util::unique_qt_ptr<QBoxLayout> selection_layout (direction);
      selection_layout->addWidget (selection_label.release());
      for (auto const& pack : puzzles_.packs())
      {
        util::unique_qt_ptr<QMenu> menu;

        for (auto const& puzzle : puzzles_.puzzlesOfPack (pack))
        {
          data::Nonogram::ID const id {pack, puzzle};

          auto const solved (puzzles_.hasBeenSolved (id));
          add_nonogram (menu.get(), id, solved);
        }

        util::unique_qt_ptr<QCommandLinkButton> button (pack.name);
        button->setMenu (menu.get());

        button->setIcon
          (puzzles_.hasBeenSolved (pack) ? solved_icon_ : unsolved_icon_);

        selection_layout->addWidget (button.release());

        nonogram_selections_.emplace
          ( std::piecewise_construct
          , std::forward_as_tuple (pack)
          , std::forward_as_tuple (std::move (button))
          );

        nonogram_menus_.emplace
          ( std::piecewise_construct
          , std::forward_as_tuple (pack)
          , std::forward_as_tuple (std::move (menu))
          );
      }

      util::unique_qt_ptr<QHBoxLayout> main_layout;
      main_layout->setContentsMargins ({});
      main_layout->addLayout (selection_layout.release());
      setLayout (main_layout.release());
    }

    void LevelSelection::setSolved (data::Nonogram::ID id)
    {
      puzzle_actions_.at (id)->setIcon (solved_icon_);

      if (puzzles_.hasBeenSolved (id.pack))
      {
        nonogram_selections_.at (id.pack)->setIcon (solved_icon_);
      }
    }
  }
}
