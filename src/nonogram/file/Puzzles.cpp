#include <nonogram/file/Puzzles.hpp>

#include <nonogram/data/Array2D.hpp>

#include <QtCore/QCoreApplication>
#include <QtCore/QDirIterator>
#include <QtCore/QTextStream>
#include <QtWidgets/QMessageBox>

namespace nonogram
{
  namespace file
  {
    QString Puzzles::internalPackName()
    {
      return "Internal";
    }

    Puzzles::Puzzles()
    : root_path_ (QCoreApplication::applicationDirPath())
    , answers_path_ (root_path_ + "/../answers")
    , puzzles_path_ (root_path_ + "/../puzzles")
    , puzzles_()
    {
      QDirIterator puzzles_directory_it
        (puzzles_path_ , QDir::AllDirs | QDir::NoDotAndDotDot);
      while (puzzles_directory_it.hasNext())
      {
        QDir const directory (puzzles_directory_it.next());
        auto const pack_name (directory.dirName());
        puzzles_.emplace (pack_name, Files{});

        QDirIterator file_it (directory.path(), QDir::Files);
        while (file_it.hasNext())
        {
          QFileInfo const file (file_it.next());
          if (file.suffix() == "sgriddler")
          {
            puzzles_.at (pack_name).emplace
              ( std::piecewise_construct
              , std::forward_as_tuple (file.baseName())
              , std::forward_as_tuple (file)
              );
          }
        }
      }

      QDirIterator answers_directory_it
        (answers_path_, QDir::AllDirs | QDir::NoDotAndDotDot);
      while (answers_directory_it.hasNext())
      {
        QDir const directory (answers_directory_it.next());
        auto const pack_name (directory.dirName());

        if (!puzzles_.count (pack_name))
        {
          continue;
        }

        QDirIterator file_it (directory.path(), QDir::Files);
        while (file_it.hasNext())
        {
          QFileInfo const file (file_it.next());
          auto const puzzle_name (file.baseName());

          if (!puzzles_.at (pack_name).count (puzzle_name))
          {
            continue;
          }

          puzzles_.at (pack_name).at (puzzle_name).answer = file;
        }
      }
    }

    data::Nonogram Puzzles::titleNonogram() const
    {
      return
        { internalPackName()
        , "Title"
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

    bool Puzzles::hasAnswer (QString pack, QString puzzle) const
    {
      return puzzles_.at (pack).at (puzzle).answer.has_value();
    }

    namespace
    {
      data::Answer::Datum toDatum (QString character)
      {
        return character == "O"
               ? data::Answer::Datum::Filled
               : character == "X"
                 ? data::Answer::Datum::Crossed
                 : character == "o"
                   ? data::Answer::Datum::FillMark
                   : character == "x"
                     ? data::Answer::Datum::CrossMark
                     : data::Answer::Datum::Empty;
      }

      QString toString (data::Answer::Datum datum)
      {
        return datum == data::Answer::Datum::Filled
               ? "O"
               : datum == data::Answer::Datum::Crossed
                 ? "X"
                 : datum == data::Answer::Datum::FillMark
                   ? "o"
                   : datum == data::Answer::Datum::CrossMark
                     ? "x"
                     : "-";
      }
    }

    data::Answer Puzzles::answer (QString pack, QString puzzle) const
    {
      auto const file_info (puzzles_.at (pack).at (puzzle).answer.value());
      if (!file_info.exists())
      {
        QMessageBox::critical
          ( nullptr
          , "File not found!"
          , QString ("Answer file %1% not found!").arg (puzzle)
          );
      }

      QFile file (file_info.filePath());
      file.open (QIODevice::ReadOnly);

      QTextStream in (&file);

      in.skipWhiteSpace();

      QString const data_dimensions_line (in.readLine());
      QStringList const data_dimensions (data_dimensions_line.split (" "));

      data::Columns const columns {data_dimensions.at (0).toUInt()};
      data::Rows const rows {data_dimensions.at (1).toUInt()};

      data::Answer::Data data (columns, rows);
      data::Answer::DataLocks data_locks (columns, rows);

      in.skipWhiteSpace();

      for (data::Row row {0}; row.value < data.rows().value; ++row.value)
      {
        QString const line (in.readLine());
        if (line.isEmpty())
        {
          continue;
        }

        QStringList const datum_list (line.split (" ", QString::SkipEmptyParts));

        for ( data::Column column {0}
            ; column.value < columns.value
            ; ++column.value
            )
        {
          QStringList const data_lock (datum_list.at (column.value).split (","));

          data.set (column, row, toDatum (data_lock.at (0)));
          data_locks.set (column, row, data_lock.at (1) == "L");
        }
      }

      in.skipWhiteSpace();

      data::Answer::CluesStates clue_states;

      for (auto const& type : data::Clues::all_types)
      {
        in.skipWhiteSpace();

        QString const clue_main_size_line (in.readLine());

        data::MainSize const main_size {clue_main_size_line.toUInt()};

        data::ClueStates::Data clues (main_size);
        data::ClueStates::Data clue_locks (main_size);

        for ( data::MainIndex main {0}
            ; main.value < clues.mainSize().value
            ; ++main.value
            )
        {
          QString const line (in.readLine());
          if (line.isEmpty())
          {
            continue;
          }

          QStringList const clue_list (line.split (" ", QString::SkipEmptyParts));

          data::MinorSize const minor_size
            {static_cast<std::size_t> (clue_list.count())};
          clues.resize (main, minor_size, false);
          clue_locks.resize (main, minor_size, false);

          for ( data::MinorIndex minor {0}
              ; minor.value < clue_list.count()
              ; ++minor.value
              )
          {
            QStringList const clue_lock (clue_list.at (minor.value).split (","));

            clues.set (main, minor, clue_lock.at (0) == "x");
            clue_locks.set (main, minor, clue_lock.at (1) == "L");
          }
        }

        clue_states.emplace
          ( std::piecewise_construct
          , std::forward_as_tuple (type)
          , std::forward_as_tuple (std::move (clues), std::move (clue_locks))
          );
      }

      file.close();

      return {data, data_locks, clue_states};
    }

    void Puzzles::writeAnswer ( QString pack
                              , QString puzzle
                              , data::Nonogram const& nonogram
                              )
    {
      auto const& answer (nonogram.answer_);

      QDir directory (answers_path_);
      if (!directory.exists (pack))
      {
        directory.mkdir (pack);
      }
      directory.cd (pack);

      auto const name (puzzles_.at (pack).at (puzzle).puzzle.baseName());

      QFile file (directory.absoluteFilePath (name));
      file.open (QIODevice::WriteOnly);

      puzzles_.at (pack).at (puzzle).answer = file;

      QTextStream out (&file);

      out << answer.data_.columns().value << " "
          << answer.data_.rows().value << "\n";

      for (data::Row row {0}; row.value < answer.data_.rows().value; ++row.value)
      {
        for ( data::Column column {0}
            ; column.value < answer.data_.columns().value
            ; ++column.value
            )
        {
          data::Slot const slot {column, row};
          out << toString (answer.at (slot)) << ","
              << (answer.isDatumLocked (slot) ? "L" : "U") << " ";
        }
        out << "\n";
      }

      for (auto const& type : data::Clues::all_types)
      {
        out << answer.clue_states_.at (type).data_.mainSize().value << "\n";

        for ( data::MainIndex main {0}
            ; main.value < answer.clue_states_.at (type).data_.mainSize().value
            ; ++main.value
            )
        {
          for ( data::MinorIndex minor {0}
              ; minor.value
                < answer.clue_states_.at (type).data_.minorSize (main).value
              ; ++minor.value
              )
          {
            data::FullIndex const full_index {main, minor};
            out << ( answer.clue_states_.at (type).isCrossed (full_index)
                   ? "x"
                   : "-"
                   ) << ","
                << ( answer.clue_states_.at (type).isLocked (full_index)
                   ? "L"
                   : "U"
                   )
                << " ";
          }
          out << "\n";
        }
      }

      file.close();
    }

    data::Nonogram Puzzles::puzzle (QString pack, QString puzzle) const
    {
      auto const file_info (puzzles_.at (pack).at (puzzle).puzzle);
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

      if (hasAnswer (pack, puzzle))
      {
        return { pack
               , puzzle
               , std::move (data::Solution (data))
               , answer (pack, puzzle)
               };
      }
      else
      {
        return {pack, puzzle, std::move (data::Solution (data))};
      }
    }
  }
}
