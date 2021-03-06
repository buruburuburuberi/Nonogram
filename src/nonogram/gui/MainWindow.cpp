#include <nonogram/gui/MainWindow.hpp>

#include <nonogram/gui/painting.hpp>

#include <QtCore/QPointF>
#include <QtCore/QTimer>
#include <QtGui/QKeySequence>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QSizePolicy>
#include <QtWidgets/QWidget>

#include <utility>

namespace nonogram::gui
{
  MainWindow::MainWindow()
  : icon_size_ (22, 22)
  , bg_color_ (Qt::yellow)
  , fg_color_ (Qt::black)
  , puzzles_()
  , start_menu_ (puzzles_, this)
  , undo_stack_()
  , current_nonogram_ (puzzles_.titleNonogram())
  , level_selection_ (puzzles_, QBoxLayout::LeftToRight)
  , level_selection_toolbar_ (addToolBar ("Level Selection"))
  , tools_toolbar_ (addToolBar ("Tools"))
  , play_field_ (bg_color_, fg_color_, undo_stack_, current_nonogram_)
  {
    connect ( &start_menu_
            , &StartMenu::nonogramSelected
            , this
            , &MainWindow::showLevel
            );
    connect ( &start_menu_
            , &StartMenu::tutorialSelected
            , this
            , &MainWindow::showTutorial
            );
    connect ( level_selection_.get()
            , &LevelSelection::levelSelected
            , this
            , &MainWindow::showLevel
            );

    check_button_->setText ("Check");
    check_button_->setDisabled (true);
    check_button_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
    check_button_->setIcon (createCheckIcon (icon_size_, bg_color_, fg_color_));
    check_button_->setShortcut (QKeySequence (Qt::Key_C));

    lock_button_->setText ("Lock");
    lock_button_->setDisabled (true);
    lock_button_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
    lock_button_->setIcon (createLockIcon (icon_size_, bg_color_, fg_color_));
    lock_button_->setShortcut (QKeySequence (Qt::Key_L));

    unlock_button_->setText ("Unlock");
    unlock_button_->setDisabled (true);
    unlock_button_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
    unlock_button_->setIcon (createUnlockIcon (icon_size_, bg_color_, fg_color_));
    unlock_button_->setShortcut (QKeySequence (Qt::Key_U));

    reset_button_->setText ("Reset");
    reset_button_->setDisabled (true);
    reset_button_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
    reset_button_->setIcon (createResetIcon (icon_size_, bg_color_, fg_color_));
    reset_button_->setShortcut (QKeySequence (Qt::Key_R));

    fill_button_->setText ("Fill");
    fill_button_->setDisabled (true);
    fill_button_->setCheckable (true);
    fill_button_->setChecked (true);
    fill_button_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
    fill_button_->setIcon
      ( createIcon ( data::Answer::Datum::Filled
                   , icon_size_
                   , bg_color_
                   , fg_color_
                   )
      );
    fill_button_->setShortcut (QKeySequence (Qt::Key_F));

    cross_button_->setText ("Cross");
    cross_button_->setDisabled (true);
    cross_button_->setCheckable (true);
    cross_button_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
    cross_button_->setIcon
      ( createIcon ( data::Answer::Datum::Crossed
                   , icon_size_
                   , bg_color_
                   , fg_color_
                   )
      );
    cross_button_->setShortcut (QKeySequence (Qt::Key_X));

    fill_mark_button_->setText ("Fill Mark");
    fill_mark_button_->setDisabled (true);
    fill_mark_button_->setCheckable (true);
    fill_mark_button_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
    fill_mark_button_->setIcon
      ( createIcon ( data::Answer::Datum::FillMark
                   , icon_size_
                   , bg_color_
                   , fg_color_
                   )
      );
    fill_mark_button_->setShortcut (QKeySequence ("Alt+F"));

    cross_mark_button_->setText ("Cross Mark");
    cross_mark_button_->setDisabled (true);
    cross_mark_button_->setCheckable (true);
    cross_mark_button_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
    cross_mark_button_->setIcon
      ( createIcon ( data::Answer::Datum::CrossMark
                   , icon_size_
                   , bg_color_
                   , fg_color_
                   )
      );
    cross_mark_button_->setShortcut (QKeySequence ("Alt+X"));

    undo_button_->setText ("Undo");
    undo_button_->setDisabled (true);
    undo_button_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
    undo_button_->setIcon (createUndoIcon (icon_size_, bg_color_, fg_color_));
    undo_button_->setShortcut (QKeySequence::Undo);

    redo_button_->setText ("Redo");
    redo_button_->setDisabled (true);
    redo_button_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
    redo_button_->setIcon (createRedoIcon (icon_size_, bg_color_, fg_color_));
    redo_button_->setShortcut (QKeySequence::Redo);

    solve_button_->setText ("Show Solution");
    solve_button_->setDisabled (true);
    solve_button_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
    solve_button_->setIcon (createSolveIcon (icon_size_, bg_color_, fg_color_));
    solve_button_->setShortcut (QKeySequence (Qt::Key_S));

    controls_button_->setText ("Show Controls");
    controls_button_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
    controls_button_->setIcon
      (createControlsIcon (icon_size_, bg_color_, fg_color_));
    controls_button_->setShortcut (QKeySequence::HelpContents);

    tools_group_->setExclusive (true);
    tools_group_->addButton (fill_button_.get());
    tools_group_->addButton (cross_button_.get());
    tools_group_->addButton (fill_mark_button_.get());
    tools_group_->addButton (cross_mark_button_.get());

    util::unique_qt_ptr<QWidget> space_left1;
    space_left1->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);
    util::unique_qt_ptr<QWidget> space_right1;
    space_right1->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);

    level_selection_toolbar_->addWidget (space_left1.release());
    level_selection_toolbar_->addWidget (level_selection_.release());
    level_selection_toolbar_->addWidget (space_right1.release());
    level_selection_toolbar_->hide();

    addToolBar (Qt::BottomToolBarArea, tools_toolbar_);
    tools_toolbar_->hide();

    util::unique_qt_ptr<QWidget> space_left2;
    space_left2->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);
    util::unique_qt_ptr<QWidget> space_right2;
    space_right2->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);
    tools_toolbar_->addWidget (space_left2.release());
    tools_toolbar_->addWidget (check_button_.release());
    tools_toolbar_->addWidget (fill_button_.release());
    tools_toolbar_->addWidget (cross_button_.release());
    tools_toolbar_->addWidget (fill_mark_button_.release());
    tools_toolbar_->addWidget (cross_mark_button_.release());
    tools_toolbar_->addWidget (undo_button_.release());
    tools_toolbar_->addWidget (redo_button_.release());
    tools_toolbar_->addWidget (lock_button_.release());
    tools_toolbar_->addWidget (unlock_button_.release());
    tools_toolbar_->addWidget (reset_button_.release());
    tools_toolbar_->addWidget (solve_button_.release());
    tools_toolbar_->addWidget (controls_button_.release());
    tools_toolbar_->addWidget (space_right2.release());

    scroll_area_->setMinimumSize (1440, 720);
    scroll_area_->setWidgetResizable (true);
    scroll_area_->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);
    scroll_area_->setWidget (play_field_.release());
    setCentralWidget (scroll_area_.release());

    connect ( play_field_.get()
            , &PlayField::panned
            , this
            , [&] (QPointF delta)
              {
                auto const horizontal_range
                  ( scroll_area_->horizontalScrollBar()->maximum()
                  - scroll_area_->horizontalScrollBar()->minimum()
                  );
                auto const vertical_range
                  ( scroll_area_->verticalScrollBar()->maximum()
                  - scroll_area_->verticalScrollBar()->minimum()
                  );
                scroll_area_->horizontalScrollBar()->setValue
                  ( scroll_area_->horizontalScrollBar()->value()
                  - delta.x() * horizontal_range
                  );
                scroll_area_->verticalScrollBar()->setValue
                  ( scroll_area_->verticalScrollBar()->value()
                  - delta.y() * vertical_range
                  );
              }
            );
    connect ( play_field_.get()
            , &PlayField::solved
            , this
            , [&]
              {
                reset (true);

                if (!current_nonogram_.id().internal())
                {
                  puzzles_.markAsSolved (current_nonogram_.id());
                  level_selection_->markAsSolved (current_nonogram_.id());
                }

                QMessageBox::information ( this
                                         , "Congratulations!"
                                         , "You have solved this puzzle."
                                         );
              }
            );

    connect ( check_button_.get()
            , &QToolButton::clicked
            , this
            , [&] { play_field_->checkAnswer(); }
            );
    connect ( lock_button_.get()
            , &QToolButton::clicked
            , this
            , [&]
              {
                play_field_->lock();
                unlock_button_->setDisabled (true);
                unlock_button_->setEnabled (true);
              }
            );
    connect ( unlock_button_.get()
            , &QToolButton::clicked
            , this
            , [&]
              {
                play_field_->unlock();
                unlock_button_->setDisabled (true);
              }
            );
    connect ( reset_button_.get()
            , &QToolButton::clicked
            , this
            , [&]
              {
                if ( QMessageBox::question
                       ( this
                       , "Restart?"
                       , "Are you sure you want to restart this puzzle?"
                       ) == QMessageBox::Yes
                   )
                {
                  play_field_->resetAnswer();
                  reset (false);
                }
              }
            );
    connect ( solve_button_.get()
            , &QToolButton::clicked
            , this
            , [&]
              {
                if ( QMessageBox::question
                       ( this
                       , "Show solution?"
                       , "Are you sure you want to show the solution?"
                       ) == QMessageBox::Yes
                   )
                {
                  play_field_->resetAnswer();
                  play_field_->showSolution (PlayField::Animate::No);
                  reset (true);
                }
              }
            );
    connect
      ( controls_button_.get()
      , &QToolButton::clicked
      , this
      , [&]
        {
          QMessageBox::information
          ( this
          , "Controls"
          , "<table>"
            "  <tr>"
            "    <td><b>LMB Click</b>:</td>"
            "    <td>Fill in a square.</td>"
            "  </tr>"
            "  <tr>"
            "    <td><b>LMB Drag</b>:</td>"
            "    <td>Fill in multiple squares in one go.</td>"
            "  </tr>"
            "  <tr>"
            "    <td><b>RMB Drag</b>:</td>"
            "    <td>Pan across a puzzle (when scrollbars are visible).</td>"
            "  </tr>"
            "</table>"
          );
        }
      );
    connect ( fill_button_.get()
            , &QToolButton::toggled
            , this
            , [&] (bool state)
              {
                if (state)
                {
                  play_field_->setFillMode (data::Answer::Datum::Filled);
                }
              }
            );
    connect ( cross_button_.get()
            , &QToolButton::toggled
            , this
            , [&] (bool state)
              {
                if (state)
                {
                  play_field_->setFillMode (data::Answer::Datum::Crossed);
                }
              }
            );
    connect ( fill_mark_button_.get()
            , &QToolButton::toggled
            , this
            , [&] (bool state)
              {
                if (state)
                {
                  play_field_->setFillMode (data::Answer::Datum::FillMark);
                }
              }
            );
    connect ( cross_mark_button_.get()
            , &QToolButton::toggled
            , this
            , [&] (bool state)
              {
                if (state)
                {
                  play_field_->setFillMode (data::Answer::Datum::CrossMark);
                }
              }
            );

    connect ( undo_button_.get()
            , &QToolButton::clicked
            , play_field_.get()
            , &PlayField::undo
            );
    connect ( &undo_stack_
            , &QUndoStack::canUndoChanged
            , this
            , [&] (bool can_undo)
              {
                if (current_nonogram_.id().internal())
                {
                  return;
                }

                undo_button_->setEnabled (can_undo);
              }
            );

    connect ( redo_button_.get()
            , &QToolButton::clicked
            , play_field_.get()
            , &PlayField::redo
            );
    connect ( &undo_stack_
            , &QUndoStack::canRedoChanged
            , this
            , [this] (bool can_redo)
              {
                if (current_nonogram_.id().internal())
                {
                  return;
                }

                redo_button_->setEnabled (can_redo);
              }
            );
    connect ( &undo_stack_
            , &QUndoStack::indexChanged
            , this
            , [&] (int index)
              {
                if (current_nonogram_.id().internal())
                {
                  return;
                }

                if (index == undo_stack_.cleanIndex())
                {
                  lock_button_->setEnabled (false);
                  unlock_button_->setEnabled (false);
                  reset_button_->setDisabled (false);
                }
                else
                {
                  lock_button_->setEnabled (play_field_->canLock());
                  unlock_button_->setEnabled (play_field_->canUnlock());
                  reset_button_->setDisabled (play_field_->isEmpty());
                }
              }
            );

    QTimer::singleShot
      ( 0
      , [&]
        {
          play_field_->showSolution (PlayField::Animate::Yes);
          start_menu_.exec();
        }
      );
  }

  void MainWindow::closeEvent (QCloseEvent*)
  {
    writeOutCurrentAnswer();
  }

  void MainWindow::writeOutCurrentAnswer()
  {
    if (current_nonogram_.id().internal() || current_nonogram_.isEmpty())
    {
      return;
    }

    puzzles_.writeAnswer (current_nonogram_.id(), current_nonogram_);
  }

  void MainWindow::reset (bool solved)
  {
    undo_stack_.clear();

    check_button_->setDisabled (solved);
    fill_button_->setDisabled (solved);
    cross_button_->setDisabled (solved);
    fill_mark_button_->setDisabled (solved);
    cross_mark_button_->setDisabled (solved);
    undo_button_->setDisabled (true);
    redo_button_->setDisabled (true);
    lock_button_->setDisabled (solved || !current_nonogram_.canLock());
    unlock_button_->setDisabled (solved || !current_nonogram_.canUnlock());
    reset_button_->setDisabled (play_field_->isEmpty());
    solve_button_->setDisabled (solved);
  }

  void MainWindow::showLevel (data::Nonogram::ID id)
  {
    setWindowTitle (QString ("%1: %2").arg (id.pack.name).arg (id.puzzle.name));
    writeOutCurrentAnswer();

    current_nonogram_ = puzzles_.puzzle (id);
    play_field_->setNonogram (current_nonogram_);

    level_selection_toolbar_->show();
    tools_toolbar_->show();

    reset (false);
  }

  void MainWindow::showTutorial()
  {
    writeOutCurrentAnswer();

    current_nonogram_ = puzzles_.tutorialNonogram();
    play_field_->setNonogram (current_nonogram_);

    level_selection_toolbar_->show();
    tools_toolbar_->show();

    check_button_->setEnabled (false);
    fill_button_->setEnabled (true);
    cross_button_->setEnabled (true);
    fill_mark_button_->setEnabled (true);
    cross_mark_button_->setEnabled (true);
    undo_button_->setEnabled (false);
    redo_button_->setEnabled (false);
    lock_button_->setEnabled (false);
    unlock_button_->setEnabled (false);
    reset_button_->setEnabled (false);
    solve_button_->setEnabled (false);

    play_field_->showTutorial();
  }
}
