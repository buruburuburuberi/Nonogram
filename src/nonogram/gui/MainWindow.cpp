#include <nonogram/gui/MainWindow.hpp>

#include <QtGui/QStandardItem>
#include <QtWidgets/QFormLayout>
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
      setMinimumWidth (800);
      setMinimumHeight (600);

      data::Nonogram nonogram
        ( "5x5"
        , { 0, 0, 1, 0, 0
          , 0, 1, 1, 1, 0
          , 1, 1, 1, 1, 1
          , 0, 1, 1, 1, 0
          , 0, 0, 1, 0, 0
          }
        );
      nonograms_.emplace (nonogram.name(), nonogram);

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

      play_field_->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);

      level_selection_layout->addRow ( "Available puzzle:"
                                     , nonogram_list_.release()
                                     );
      level_selection_widget->setLayout (level_selection_layout.release());

      QToolBar* toolbar (addToolBar ("Tools"));
      toolbar->addWidget (level_selection_widget.release());

      setCentralWidget (play_field_.release());
    }
  }
}
