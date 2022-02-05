#include <nonogram/gui/MainWindow.hpp>

#include <nonogram/gui/painting.hpp>

#include <QtCore/QTimer>
#include <QtGui/QStandardItem>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

#include <utility>

namespace nonogram
{
  namespace gui
  {
    MainWindow::MainWindow (std::map<std::string, data::Nonogram> nonograms)
    : icon_size_ (22, 22)
    , bg_color_ (Qt::yellow)
    , fg_color_ (Qt::black)
    , undo_stack_()
    , nonograms_ (std::move (nonograms))
    , play_field_ (bg_color_, fg_color_, undo_stack_, std::move (titleNonogram()))
    {
      statusBar()->showMessage ("Ready");

      util::unique_qt_ptr<QFrame> level_selection_widget;
      util::unique_qt_ptr<QFormLayout> level_selection_layout;

      QList<QStandardItem*> items;
      auto add_item
      ( [&] (QString name, bool enabled)
        {
          auto item (new QStandardItem (name));
          item->setFlags
            ( enabled
            ? Qt::ItemIsSelectable | Qt::ItemIsEnabled
            : Qt::ItemIsEnabled
            );
          items.append (item);
        }
      );

      add_item ("---Select Level---", false);
      for (auto const& nonogram : nonograms_)
      {
        add_item (nonogram.first.data(), true);
      }
      nonogram_model_.appendColumn (items);

      nonogram_list_->setModel (&nonogram_model_);

      connect ( nonogram_list_.get()
              , &QComboBox::currentTextChanged
              , this
              , [&] (QString text)
                {
                  play_field_->setNonogram (nonograms_.at (text.toStdString()));

                  tools_toolbar_->setEnabled (true);
                  solve_button_->setEnabled (true);
                }
              );

      level_selection_layout->addRow ( "Available puzzles:"
                                     , nonogram_list_.release()
                                     );
      level_selection_widget->setLayout (level_selection_layout.release());

      check_button_->setText ("Check");
      check_button_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
      check_button_->setIcon (createCheckIcon (icon_size_, bg_color_, fg_color_));

      lock_button_->setText ("Lock");
      lock_button_->setDisabled (true);
      lock_button_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
      lock_button_->setIcon (createLockIcon (icon_size_, bg_color_, fg_color_));

      unlock_button_->setText ("Unlock");
      unlock_button_->setDisabled (true);
      unlock_button_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
      unlock_button_->setIcon (createUnlockIcon (icon_size_, bg_color_, fg_color_));

      reset_button_->setText ("Reset");
      reset_button_->setDisabled (true);
      reset_button_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
      reset_button_->setIcon (createResetIcon (icon_size_, bg_color_, fg_color_));

      fill_button_->setText ("Fill");
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

      cross_button_->setText ("Cross");
      cross_button_->setCheckable (true);
      cross_button_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
      cross_button_->setIcon
        ( createIcon ( data::Answer::Datum::Crossed
                     , icon_size_
                     , bg_color_
                     , fg_color_
                     )
        );

      fill_mark_button_->setText ("Fill Mark");
      fill_mark_button_->setCheckable (true);
      fill_mark_button_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
      fill_mark_button_->setIcon
        ( createIcon ( data::Answer::Datum::FillMark
                     , icon_size_
                     , bg_color_
                     , fg_color_
                     )
        );

      cross_mark_button_->setText ("Cross Mark");
      cross_mark_button_->setCheckable (true);
      cross_mark_button_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
      cross_mark_button_->setIcon
        ( createIcon ( data::Answer::Datum::CrossMark
                     , icon_size_
                     , bg_color_
                     , fg_color_
                     )
        );

      undo_button_->setText ("Undo");
      undo_button_->setEnabled (false);
      undo_button_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
      undo_button_->setIcon (createUndoIcon (icon_size_, bg_color_, fg_color_));

      redo_button_->setText ("Redo");
      redo_button_->setEnabled (false);
      redo_button_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
      redo_button_->setIcon (createRedoIcon (icon_size_, bg_color_, fg_color_));

      solve_button_->setText ("Show Solution");
      solve_button_->setEnabled (false);
      solve_button_->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
      solve_button_->setIcon (createSolveIcon (icon_size_, bg_color_, fg_color_));

      tools_group_->setExclusive (true);
      tools_group_->addButton (fill_button_.get());
      tools_group_->addButton (cross_button_.get());
      tools_group_->addButton (fill_mark_button_.get());
      tools_group_->addButton (cross_mark_button_.get());

      QToolBar* level_selection_toolbar (addToolBar ("Level Selection"));
      level_selection_toolbar->addWidget (level_selection_widget.release());

      addToolBarBreak();

      tools_toolbar_ = addToolBar ("Tools");

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

      tools_toolbar_->setDisabled (true);

      setCentralWidget (play_field_.release());

      connect ( play_field_.get()
              , &PlayField::solved
              , this
              , [&] { tools_toolbar_->setDisabled (true); }
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
                    play_field_->showSolution();
                  }
                }
              );
      connect ( fill_button_.get()
              , &QToolButton::toggled
              , this
              , [&]
                { play_field_->setFillMode (data::Answer::Datum::Filled); }
              );
      connect ( cross_button_.get()
              , &QToolButton::toggled
              , this
              , [&]
                { play_field_->setFillMode (data::Answer::Datum::Crossed); }
              );
      connect ( fill_mark_button_.get()
              , &QToolButton::toggled
              , this
              , [&]
                { play_field_->setFillMode (data::Answer::Datum::FillMark); }
              );
      connect ( cross_mark_button_.get()
              , &QToolButton::toggled
              , this
              , [&]
                { play_field_->setFillMode (data::Answer::Datum::CrossMark); }
              );

      connect ( undo_button_.get()
              , &QToolButton::clicked
              , play_field_.get()
              , &PlayField::undo
              );
      connect ( &undo_stack_
              , &QUndoStack::canUndoChanged
              , this
              , [&] (bool can_undo) { undo_button_->setEnabled (can_undo); }
              );

      connect ( redo_button_.get()
              , &QToolButton::clicked
              , play_field_.get()
              , &PlayField::redo
              );
      connect ( &undo_stack_
              , &QUndoStack::canRedoChanged
              , this
              , [this] (bool can_redo) { redo_button_->setEnabled (can_redo); }
              );

      connect ( &undo_stack_
              , &QUndoStack::cleanChanged
              , this
              , [&] (bool clean)
                {
                  lock_button_->setDisabled (clean);
                  reset_button_->setDisabled (clean);

                  if (clean)
                  {
                    unlock_button_->setDisabled (true);
                  }
                }
              );

      connect ( &undo_stack_
              , &QUndoStack::indexChanged
              , this
              , [&] (int)
                {
                  lock_button_->setEnabled (play_field_->canLock());
                  unlock_button_->setEnabled (play_field_->canUnlock());
                }
              );

      QTimer::singleShot (0, [&] { play_field_->showSolution(); });
    }

    data::Nonogram MainWindow::titleNonogram() const
    {
      return
        { "Title"
        , data::Array2D<bool>
          { { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
            , { 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0 }
            , { 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0 }
            , { 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0 }
            , { 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0 }
            , { 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0 }
            , { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
            , { 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0 }
            , { 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0 }
            , { 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0 }
            , { 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0 }
            , { 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0 }
            , { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
            }
          }
        };
    }
  }
}
