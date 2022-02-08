#include <nonogram/gui/MainWindow.hpp>

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
  QApplication application (argc, argv);

  nonogram::gui::MainWindow appWindow;
  appWindow.show();

  return application.exec();
}
