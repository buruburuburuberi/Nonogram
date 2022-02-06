#include <nonogram/gui/MainWindow.hpp>

#include <nonogram/util/print_exception.hpp>

#include <QtWidgets/QApplication>

#include <iostream>
#include <stdexcept>

int main(int argc, char *argv[])
try
{
  QApplication application (argc, argv);

  nonogram::gui::MainWindow appWindow;
  appWindow.show();

  return application.exec();
}
catch (...)
{
  std::cerr << "EXCEPTION: "
            << nonogram::util::exception_printer (std::current_exception())
            << std::endl;

  return 1;
}
