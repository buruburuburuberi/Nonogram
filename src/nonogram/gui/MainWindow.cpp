#include <nonogram/gui/MainWindow.hpp>

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
    : undo_stack_()
    , nonograms_ (std::move (nonograms))
    , play_field_ (undo_stack_, nonograms_.begin()->second)
    {
      statusBar()->showMessage ("Ready");

      play_field_->setVisible (false);

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
                  play_field_->setVisible (true);
                  play_field_->setNonogram (nonograms_.at (text.toStdString()));

                  tools_toolbar_->setEnabled (true);
                }
              );

      level_selection_layout->addRow ( "Available puzzles:"
                                     , nonogram_list_.release()
                                     );
      level_selection_widget->setLayout (level_selection_layout.release());

      check_button_->setText ("Check");

      reset_button_->setText ("Reset");
      reset_button_->setDisabled (true);

      fill_button_->setText ("Fill");
      fill_button_->setCheckable (true);
      fill_button_->setChecked (true);

      cross_button_->setText ("Cross");
      cross_button_->setCheckable (true);

      fill_mark_button_->setText ("Fill Mark");
      fill_mark_button_->setCheckable (true);

      cross_mark_button_->setText ("Cross Mark");
      cross_mark_button_->setCheckable (true);

      undo_button_->setIcon (style()->standardIcon (QStyle::SP_ArrowLeft));
      undo_button_->setEnabled (false);

      redo_button_->setIcon (style()->standardIcon (QStyle::SP_ArrowRight));
      redo_button_->setEnabled (false);

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
      tools_toolbar_->addWidget (reset_button_.release());

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
              , [&] (bool clean) { reset_button_->setDisabled (clean); }
              );
    }
  }
}
