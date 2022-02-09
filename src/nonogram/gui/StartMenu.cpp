#include <nonogram/gui/StartMenu.hpp>

#include <QtCore/QSettings>
#include <QtGui/QGuiApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>

#include <iostream>

namespace nonogram
{
  namespace gui
  {
    StartMenu::StartMenu (file::Puzzles const& puzzles, QWidget* _parent)
    : QDialog (_parent)
    , continue_button_ ("Continue")
    , level_selection_ (puzzles, QBoxLayout::TopToBottom)
    , quit_button_ ("Quit")
    {
      QSettings settings;
      auto const has_last_puzzle
        ( settings.contains ("current_pack")
       && settings.contains ("current_puzzle")
        );

      continue_button_->setEnabled (has_last_puzzle);

      connect ( continue_button_.get()
              , &QCommandLinkButton::clicked
              , this
              , [&]
                {
                  QSettings settings;

                  emit nonogramSelected
                      ( settings.value ("current_pack").toString()
                      , settings.value ("current_puzzle").toString()
                      );
                  accept();
                }
              );

      connect ( level_selection_.get()
              , &LevelSelection::levelSelected
              , this
              , [&] (QString pack, QString puzzle)
                {
                  emit nonogramSelected (pack, puzzle);
                  accept();
                }
              );

      connect ( quit_button_.get()
              , &QCommandLinkButton::clicked
              , this
              , [&]
                {
                  QGuiApplication::quit();
                }
              );

      util::unique_qt_ptr<QVBoxLayout> main_layout;
      main_layout->addWidget (continue_button_.release());
      main_layout->addWidget (level_selection_.release());
      main_layout->addWidget (quit_button_.release());

      setLayout (main_layout.release());
    }
  }
}
