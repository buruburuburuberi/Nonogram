#include <nonogram/gui/StartMenu.hpp>

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
    , puzzles_ (puzzles)
    , current_puzzle_ (puzzles_.currentPuzzle())
    , continue_button_ ("Continue")
    , level_selection_ (puzzles, QBoxLayout::TopToBottom)
    , quit_button_ ("Quit")
    {
      setWindowFlag (Qt::FramelessWindowHint, true);

      continue_button_->setDescription ("Continue where you left off...");
      continue_button_->setEnabled (current_puzzle_.has_value());

      util::unique_qt_ptr<QFrame> line1;
      line1->setFrameShape(QFrame::HLine);

      util::unique_qt_ptr<QFrame> line2;
      line2->setFrameShape(QFrame::HLine);

      util::unique_qt_ptr<QVBoxLayout> main_layout;
      main_layout->addWidget (continue_button_.release());
      main_layout->addWidget (line1.release());
      main_layout->addWidget (level_selection_.release());
      main_layout->addWidget (line2.release());
      main_layout->addWidget (quit_button_.release());

      setLayout (main_layout.release());

      connect ( continue_button_.get()
              , &QCommandLinkButton::clicked
              , this
              , [&]
                {
                  emit nonogramSelected (current_puzzle_.value());
                  accept();
                }
              );

      connect ( level_selection_.get()
              , &LevelSelection::levelSelected
              , this
              , [&] (data::Nonogram::ID id)
                {
                  emit nonogramSelected (id);
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
    }
  }
}
