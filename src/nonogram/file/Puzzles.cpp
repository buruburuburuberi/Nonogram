#include <nonogram/file/Puzzles.hpp>

#include <nonogram/data/Array2D.hpp>

#include <QtCore/QDirIterator>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtWidgets/QMessageBox>

#include <iostream>

namespace nonogram
{
  namespace file
  {
    Puzzles::Puzzles (QString root_path)
    : puzzles_ ()
    {
      QDirIterator directory_it (root_path, QDir::AllDirs | QDir::NoDotAndDotDot);
      while (directory_it.hasNext())
      {
        QDir const directory (directory_it.next());
        auto const directoryName (directory.dirName());
        puzzles_.emplace (directoryName, Files{});

        QDirIterator file_it (directory.path(), QDir::Files);
        while (file_it.hasNext())
        {
          QFileInfo const file (file_it.next());
          if (file.suffix() == "sgriddler")
          {
            puzzles_.at (directoryName).emplace (file.baseName(), file);
          }
        }
      }
    }

    data::Nonogram Puzzles::titleNonogram() const
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

    QStringList Puzzles::packs() const
    {
      QStringList packs;

      std::transform
        ( puzzles_.begin()
        , puzzles_.end()
        , std::back_inserter (packs)
        , [] (const Directories::value_type& pack) { return pack.first; }
        );

      return packs;
    }

    QStringList Puzzles::puzzlesOfPack (QString pack) const
    {
      QStringList puzzles;

      std::transform
        ( puzzles_.at (pack).begin()
        , puzzles_.at (pack).end()
        , std::back_inserter (puzzles)
        , [] (const Files::value_type& file) { return file.first; }
        );

      return puzzles;
    }

    data::Nonogram Puzzles::puzzle (QString pack, QString puzzle) const
    {
      auto const file_info (puzzles_.at (pack).at (puzzle));
      if (!file_info.exists())
      {
        QMessageBox::critical
          ( nullptr
          , "File not found!"
          , QString ("Puzzle file %1% not found!").arg (puzzle)
          );
      }

      QFile file (file_info.filePath());
      file.open (QIODevice::ReadOnly);

      QTextStream in (&file);

      QString const dimensions_line (in.readLine());
      QStringList const dimensions (dimensions_line.split (" "));

      data::Array2D<bool> data
        ( data::Columns {dimensions.at (0).toUInt()}
        , data::Rows {dimensions.at (1).toUInt()}
        );

      data::Row row {0};
      while (!in.atEnd())
      {
        QString const line (in.readLine());
        if (line.isEmpty())
        {
          continue;
        }

        QStringList const datum_list (line.split (" ", QString::SkipEmptyParts));

        data::Column column {0};
        for (auto const& datum : datum_list)
        {
          data.set (column, row, datum == "#");

          column.value++;
        }
        row.value++;
      }

      file.close();

      return {puzzle.toStdString(), std::move (data::Solution (data))};
    }
  }
}
