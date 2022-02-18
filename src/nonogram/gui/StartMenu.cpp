#include <nonogram/gui/StartMenu.hpp>

#include <QtGui/QGuiApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QVBoxLayout>

namespace nonogram
{
  namespace gui
  {
    StartMenu::StartMenu (file::Puzzles const& puzzles, QWidget* _parent)
    : QDialog (_parent)
    , puzzles_ (puzzles)
    , current_puzzle_ (puzzles_.currentPuzzle())
    , continue_button_ ("Continue")
    , tutorial_button_ ("Tutorial")
    , level_selection_ (puzzles, QBoxLayout::TopToBottom)
    , quit_button_ ("Quit")
    {
      setWindowFlag (Qt::FramelessWindowHint, true);

      continue_button_->setDescription ("Continue where you left off...");
      continue_button_->setEnabled (current_puzzle_.has_value());

      tutorial_button_->setDescription ("Learn the basic rules of the game.");

      util::unique_qt_ptr<QFrame> line1;
      line1->setFrameShape (QFrame::HLine);

      util::unique_qt_ptr<QFrame> line2;
      line2->setFrameShape (QFrame::HLine);

      util::unique_qt_ptr<QFrame> line3;
      line3->setFrameShape (QFrame::HLine);

      util::unique_qt_ptr<QVBoxLayout> main_layout;
      main_layout->addWidget (continue_button_.release());
      main_layout->addWidget (line1.release());
      main_layout->addWidget (tutorial_button_.release());
      main_layout->addWidget (line2.release());
      main_layout->addWidget (level_selection_.release());
      main_layout->addWidget (line3.release());
      main_layout->addWidget (quit_button_.release());

      setLayout (main_layout.release());

      connect ( continue_button_.get()
              , &QCommandLinkButton::clicked
              , this
              , [&]
                {
                  accept();
                  emit nonogramSelected (current_puzzle_.value());
                }
              );

      connect ( tutorial_button_.get()
              , &QCommandLinkButton::clicked
              , this
              , [&]
                {
                  accept();
                  emit tutorialSelected();
                }
              );

      connect ( level_selection_.get()
              , &LevelSelection::levelSelected
              , this
              , [&] (data::Nonogram::ID id)
                {
                  accept();
                  emit nonogramSelected (id);
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
