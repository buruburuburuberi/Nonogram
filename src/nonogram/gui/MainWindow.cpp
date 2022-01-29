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

      QToolBar* toolbar (addToolBar ("Tools"));
      toolbar->addWidget (level_selection_widget.release());

      setCentralWidget (play_field_.release());
    }
  }
}
