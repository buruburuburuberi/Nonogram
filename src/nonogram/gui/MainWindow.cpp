#include <nonogram/gui/MainWindow.hpp>

#include <QtGui/QStandardItem>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

#include <utility>

namespace nonogram
{
  namespace gui
  {
    MainWindow::MainWindow()
    : play_field_ (this)
    {
      statusBar()->showMessage ("Ready");

      auto add_nonogram
        ( [&] (data::Nonogram nonogram)
          {
            nonograms_.emplace (nonogram.name(), std::move (nonogram));
          }
        );

      add_nonogram
        ( { "5x5"
          , data::Array2D<bool>
              { { { 0, 0, 1, 0, 0 }
                , { 0, 1, 1, 1, 0 }
                , { 1, 1, 0, 1, 1 }
                , { 0, 1, 1, 1, 0 }
                , { 0, 0, 1, 0, 0 }
                }
              }
          }
        );
      add_nonogram
        ( { "8x5"
          , data::Array2D<bool>
              { { { 1, 0, 0, 1, 1, 1, 0, 0 }
                , { 1, 0, 0, 1, 0, 1, 1, 0 }
                , { 1, 0, 1, 0, 1, 0, 1, 1 }
                , { 1, 0, 0, 1, 0, 1, 0, 0 }
                , { 1, 1, 0, 0, 1, 0, 0, 0 }
                }
              }
          }
        );

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
                }
              );

      level_selection_layout->addRow ( "Available puzzles:"
                                     , nonogram_list_.release()
                                     );
      level_selection_widget->setLayout (level_selection_layout.release());

      util::unique_qt_ptr<QToolButton> fill_button;
      fill_button->setText ("Fill");
      fill_button->setCheckable (true);
      fill_button->setChecked (true);

      util::unique_qt_ptr<QToolButton> erase_button;
      erase_button->setText ("Erase");
      erase_button->setCheckable (true);

      util::unique_qt_ptr<QToolButton> cross_button;
      cross_button->setText ("Cross");
      cross_button->setCheckable (true);

      util::unique_qt_ptr<QToolButton> fill_mark_button;
      fill_mark_button->setText ("Fill Mark");
      fill_mark_button->setCheckable (true);

      util::unique_qt_ptr<QToolButton> cross_mark_button;
      cross_mark_button->setText ("Cross Mark");
      cross_mark_button->setCheckable (true);

      tools_group_->setExclusive (true);
      tools_group_->addButton (fill_button.get());
      tools_group_->addButton (erase_button.get());
      tools_group_->addButton (cross_button.get());
      tools_group_->addButton (fill_mark_button.get());
      tools_group_->addButton (cross_mark_button.get());

      QToolBar* level_selection_toolbar (addToolBar ("Level Selection"));
      level_selection_toolbar->addWidget (level_selection_widget.release());

      addToolBarBreak();

      QToolBar* tools_toolbar (addToolBar ("Tools"));
      tools_toolbar->addWidget (fill_button.release());
      tools_toolbar->addWidget (erase_button.release());
      tools_toolbar->addWidget (cross_button.release());
      tools_toolbar->addWidget (fill_mark_button.release());
      tools_toolbar->addWidget (cross_mark_button.release());

      setCentralWidget (play_field_.release());

      connect ( fill_button.get()
              , &QToolButton::toggled
              , this
              , [&]
                { play_field_->setFillMode (data::Answer::Datum::Filled); }
              );
      connect ( erase_button.get()
              , &QToolButton::toggled
              , this
              , [&]
                { play_field_->setFillMode (data::Answer::Datum::Empty); }
              );
      connect ( cross_button.get()
              , &QToolButton::toggled
              , this
              , [&]
                { play_field_->setFillMode (data::Answer::Datum::Crossed); }
              );
      connect ( fill_mark_button.get()
              , &QToolButton::toggled
              , this
              , [&]
                { play_field_->setFillMode (data::Answer::Datum::FillMark); }
              );
      connect ( cross_mark_button.get()
              , &QToolButton::toggled
              , this
              , [&]
                { play_field_->setFillMode (data::Answer::Datum::CrossMark); }
              );
    }
  }
}
