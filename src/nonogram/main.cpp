#include <nonogram/gui/MainWindow.hpp>

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
  try
  {
    std::rethrow_exception (std::current_exception());
  }
  catch (const std::exception& e)
  {
    std::cerr << "EXCEPTION: " << e.what() << std::endl;
  }

  return 1;
}
