#include <nonogram/gui/MainWindow.hpp>

#include <QtWidgets/QApplication>

int main (int argc, char *argv[])
{
  QApplication application (argc, argv);
  QApplication::setOrganizationName ("Delger Lhamsuren");
  QApplication::setApplicationName ("Nonogram");

  nonogram::gui::MainWindow appWindow;
  appWindow.show();

  return application.exec();
}
