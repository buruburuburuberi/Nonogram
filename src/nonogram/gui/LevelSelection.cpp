#include <nonogram/gui/LevelSelection.hpp>

#include <QtCore/QList>
#include <QtWidgets/QAction>
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
    , solved_icon_ (style()->standardIcon (QStyle::SP_DialogApplyButton))
    , select_difficulty_text_ ("---Select Difficulty---")
    , select_nonogram_text_ ("---Select Nonogram---")
    , pack_selection_()
    , nonogram_selection_()
    , puzzles_ (puzzles)
    {
      auto add_pack
      ( [&] (QMenu* menu, data::Nonogram::Pack pack)
        {
          util::unique_qt_ptr<QAction> action (pack.name);
          if (puzzles_.hasBeenSolved (pack))
          {
            action->setIcon (solved_icon_);
          }

          connect ( action.get()
                  , &QAction::triggered
                  , this
                  , [this, pack] { setPack (pack); }
                  );


          menu->addAction (action.release());

          pack_actions_.emplace
            ( std::piecewise_construct
            , std::forward_as_tuple (pack)
            , std::forward_as_tuple (std::move (action))
            );
        }
      );

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
                  , [this, id, solved]
                    {
                      nonogram_selection_->setText (id.puzzle.name);
                      nonogram_selection_->setIcon
                        (solved ? solved_icon_ : QIcon());
                      emit levelSelected (id);
                    }
                  );


          menu->addAction (action.release());

          puzzle_actions_.emplace
            ( std::piecewise_construct
            , std::forward_as_tuple (id)
            , std::forward_as_tuple (std::move (action))
            );
        }
      );

      for (auto const& pack : puzzles_.packs())
      {
        nonogram_menus_.emplace
          ( std::piecewise_construct
          , std::forward_as_tuple (pack)
          , std::forward_as_tuple ()
          );

        for (auto const& puzzle : puzzles_.puzzlesOfPack (pack))
        {
          data::Nonogram::ID const id {pack, puzzle};

          auto const solved (puzzles_.hasBeenSolved (id));
          add_nonogram (nonogram_menus_.at (pack).get(), id, solved);
        }

        add_pack (pack_menu_.get(), pack);
      }

      util::unique_qt_ptr<QLabel> pack_label ("Difficulty:");
      pack_selection_->setText (select_difficulty_text_);
      pack_selection_->setMenu (pack_menu_.get());
      pack_selection_->setPopupMode (QToolButton::InstantPopup);
      pack_selection_->setToolButtonStyle (Qt::ToolButtonTextBesideIcon);

      util::unique_qt_ptr<QHBoxLayout> pack_selection_layout;
      pack_selection_layout->addWidget (pack_label.release());
      pack_selection_layout->addWidget (pack_selection_.release());

      util::unique_qt_ptr<QLabel> nonogram_label ("Nonogram:");
      nonogram_selection_->setText (select_nonogram_text_);
      nonogram_selection_->setPopupMode (QToolButton::InstantPopup);
      nonogram_selection_->setToolButtonStyle (Qt::ToolButtonTextBesideIcon);
      nonogram_selection_->setDisabled (true);

      util::unique_qt_ptr<QHBoxLayout> nonogram_selection_layout;
      nonogram_selection_layout->addWidget (nonogram_label.release());
      nonogram_selection_layout->addWidget (nonogram_selection_.release());

      util::unique_qt_ptr<QBoxLayout> main_layout (direction);
      main_layout->addLayout (pack_selection_layout.release());
      main_layout->addLayout (nonogram_selection_layout.release());

      setLayout (main_layout.release());
    }

    void LevelSelection::setPack (data::Nonogram::Pack pack)
    {
      pack_selection_->setText (pack.name);
      pack_selection_->setIcon
        (puzzles_.hasBeenSolved (pack) ? solved_icon_ : QIcon());
      nonogram_selection_->setText (select_nonogram_text_);
      nonogram_selection_->setMenu (nonogram_menus_.at (pack).get());
      nonogram_selection_->setEnabled (true);
    }

    void LevelSelection::setLevel (data::Nonogram::ID id)
    {
      setPack (id.pack);
      nonogram_selection_->setText (id.puzzle.name);
      nonogram_selection_->setIcon
        (puzzles_.hasBeenSolved (id) ? solved_icon_ : QIcon());
    }

    void LevelSelection::setSolved (data::Nonogram::ID id)
    {
      puzzle_actions_.at (id)->setIcon (solved_icon_);
      nonogram_selection_->setIcon (solved_icon_);

      if (puzzles_.hasBeenSolved (id.pack))
      {
        pack_actions_.at (id.pack)->setIcon (solved_icon_);
        pack_selection_->setIcon
          ( pack_selection_->text() == id.pack.name
          ? solved_icon_
          : QIcon()
          );
      }
    }
  }
}
