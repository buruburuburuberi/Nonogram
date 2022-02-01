#include <nonogram/data/Nonogram.hpp>
#include <nonogram/gui/MainWindow.hpp>

#include <QtWidgets/QApplication>

#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

int main(int argc, char *argv[])
try
{
  QApplication application (argc, argv);

  std::map<std::string, nonogram::data::Nonogram> nonograms;

  auto add_nonogram
    ( [&] (nonogram::data::Nonogram nonogram)
      {
        nonograms.emplace (nonogram.name(), std::move (nonogram));
      }
    );

  add_nonogram
    ( { "5x5"
      , nonogram::data::Array2D<bool>
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
      , nonogram::data::Array2D<bool>
          { { { 1, 0, 0, 1, 1, 1, 0, 0 }
            , { 1, 0, 0, 1, 0, 1, 1, 0 }
            , { 1, 0, 1, 0, 1, 0, 1, 1 }
            , { 1, 0, 0, 1, 0, 1, 0, 0 }
            , { 1, 1, 0, 0, 1, 0, 0, 0 }
            }
          }
      }
    );

  nonogram::gui::MainWindow appWindow (std::move (nonograms));
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
