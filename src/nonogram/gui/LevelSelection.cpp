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
    , select_difficulty_text_ ("---Select Difficulty---")
    , select_nonogram_text_ ("---Select Nonogram---")
    , pack_selection_()
    , nonogram_selection_()
    , puzzles_ (puzzles)
    {
      auto add_pack
      ( [&] (QMenu* menu, QString pack_name, bool solved)
        {
          util::unique_qt_ptr<QAction> action (pack_name);
          if (solved)
          {
            action->setIcon
              (style()->standardIcon (QStyle::SP_DialogApplyButton));
          }

          connect ( action.get()
                  , &QAction::triggered
                  , this
                  , [this, pack_name]
                    {
                      pack_selection_->setText (pack_name);
                      nonogram_selection_->setText (select_nonogram_text_);
                      nonogram_selection_->setMenu
                        (nonogram_menus_.at (pack_name).get());
                      nonogram_selection_->setEnabled (true);
                    }
                  );


          menu->addAction (action.release());
        }
      );

      auto add_nonogram
      ( [&] (QMenu* menu, QString nonogram_name, bool solved)
        {
          util::unique_qt_ptr<QAction> action (nonogram_name);
          if (solved)
          {
            action->setIcon
              (style()->standardIcon (QStyle::SP_DialogApplyButton));
          }

          connect ( action.get()
                  , &QAction::triggered
                  , this
                  , [this, nonogram_name]
                    {
                      nonogram_selection_->setText (nonogram_name);
                      emit levelSelected (pack_selection_->text(), nonogram_name);
                    }
                  );


          menu->addAction (action.release());
        }
      );

      for (auto const& pack : puzzles_.packs())
      {
        //!todo add solved check
        add_pack (pack_menu_.get(), pack, false);

        nonogram_menus_.emplace
          ( std::piecewise_construct
          , std::forward_as_tuple (pack)
          , std::forward_as_tuple ()
          );

        for (auto const& puzzle : puzzles_.puzzlesOfPack (pack))
        {
          //!todo add solved check
          add_nonogram (nonogram_menus_.at (pack).get(), puzzle, false);
        }
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

    void LevelSelection::setPack (QString pack)
    {
      pack_selection_->setText (pack);
      nonogram_selection_->setText (select_nonogram_text_);
      nonogram_selection_->setMenu (nonogram_menus_.at (pack).get());
      nonogram_selection_->setEnabled (true);
    }

    void LevelSelection::setLevel (QString pack, QString puzzle)
    {
      setPack (pack);
      nonogram_selection_->setText (puzzle);
    }
  }
}
