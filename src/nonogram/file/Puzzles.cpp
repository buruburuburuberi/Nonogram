#include <nonogram/file/Puzzles.hpp>

#include <nonogram/data/Array2D.hpp>

#include <QtCore/QCoreApplication>
#include <QtCore/QDirIterator>
#include <QtCore/QSettings>
#include <QtCore/QTextStream>
#include <QtWidgets/QMessageBox>

#include <algorithm>

namespace nonogram
{
  namespace file
  {
    data::Nonogram::Pack Puzzles::internalPackName()
    {
      return {"Internal"};
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
        data::Nonogram::Pack const pack {directory.dirName()};

        QDirIterator file_it (directory.path(), QDir::Files);
        while (file_it.hasNext())
        {
          QFileInfo const file (file_it.next());
          if (file.suffix() == "nonogram")
          {
            data::Nonogram::Puzzle const puzzle {file.baseName()};
            puzzles_.emplace
              ( std::piecewise_construct
              , std::forward_as_tuple (data::Nonogram::ID {pack, puzzle})
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
        data::Nonogram::Pack const pack {directory.dirName()};

        if ( std::none_of
              ( puzzles_.begin()
              , puzzles_.end()
              , [pack] (auto pair) { return pair.first.pack.name == pack.name; }
              )
           )
        {
          continue;
        }

        QDirIterator file_it (directory.path(), QDir::Files);
        while (file_it.hasNext())
        {
          QFileInfo const file (file_it.next());
          data::Nonogram::Puzzle const puzzle {file.baseName()};
          data::Nonogram::ID const id {pack, puzzle};

          if (!puzzles_.count (id))
          {
            continue;
          }

          puzzles_.at (id).answer = file;
        }
      }
    }

    data::Nonogram Puzzles::titleNonogram() const
    {
      return
        { {internalPackName(), data::Nonogram::Puzzle {"Title"}}
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

    data::Nonogram Puzzles::tutorialNonogram() const
    {
      return { {internalPackName(), data::Nonogram::Puzzle {"Tutorial"}}
             , data::Array2D<bool>
                 { { { 0, 0, 1, 0, 0 }
                   , { 0, 1, 1, 1, 0 }
                   , { 1, 1, 0, 1, 1 }
                   , { 0, 1, 1, 1, 0 }
                   , { 0, 0, 1, 0, 0 }
                   }
                 }
             };
    }

    data::Nonogram::Packs Puzzles::packs() const
    {
      data::Nonogram::Packs packs;

      for (auto const& puzzle : puzzles_)
      {
        packs.insert (puzzle.first.pack);
      }

      return packs;
    }

    data::Nonogram::Puzzles Puzzles::puzzlesOfPack (data::Nonogram::Pack pack) const
    {
      data::Nonogram::Puzzles puzzles;

      for (auto const& puzzle : puzzles_)
      {
        if (puzzle.first.pack == pack)
        {
          puzzles.insert (puzzle.first.puzzle);
        }
      }

      return puzzles;
    }

    bool Puzzles::hasAnswer (data::Nonogram::ID id) const
    {
      return puzzles_.at (id).answer.has_value();
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

    data::Answer Puzzles::answer (data::Nonogram::ID id) const
    {
      auto const file_info (puzzles_.at (id).answer.value());
      if (!file_info.exists())
      {
        QMessageBox::critical
          ( nullptr
          , "File not found!"
          , QString ("Answer file %1 from pack %2 not found!")
            .arg (id.puzzle.name).arg (id.pack.name)
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

    void Puzzles::writeAnswer ( data::Nonogram::ID id
                              , data::Nonogram const& nonogram
                              )
    {
      auto const pack_name (id.pack.name);
      auto const puzzle_name (id.puzzle.name);

      auto const& answer (nonogram.answer_);

      QDir directory (answers_path_);
      if (!directory.exists (pack_name))
      {
        directory.mkdir (pack_name);
      }
      directory.cd (pack_name);

      QFile file (directory.absoluteFilePath (puzzle_name));
      file.open (QIODevice::WriteOnly);

      puzzles_.at (id).answer = file;

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

      setCurrentPuzzle (id);
    }

    std::optional<data::Nonogram::ID> Puzzles::currentPuzzle() const
    {
      QSettings settings;
      auto const key (QString ("current"));
      return settings.contains (key)
           ? std::optional (data::Nonogram::ID (settings.value (key).toString()))
           : std::nullopt;

    }

    void Puzzles::setCurrentPuzzle (data::Nonogram::ID id)
    {
      QSettings settings;
      settings.setValue (QString ("current"), id.toString());
    }

    bool Puzzles::hasBeenSolved (data::Nonogram::Pack pack) const
    {
      auto const puzzles (puzzlesOfPack (pack));

      return std::all_of
          ( puzzles.begin()
          , puzzles.end()
          , [this, pack] (auto puzzle) { return hasBeenSolved ({pack, puzzle}); }
          );
    }

    bool Puzzles::hasBeenSolved (data::Nonogram::ID id) const
    {
      QSettings settings;
      auto const key (QString ("solved-%1").arg (id.toString()));
      return settings.contains (key) && settings.value (key).toBool();
    }

    void Puzzles::setSolved (data::Nonogram::ID id)
    {
      QSettings settings;
      settings.setValue ( QString ("solved-%1").arg (id.toString())
                        , true
                        );
    }

    data::Nonogram Puzzles::puzzle (data::Nonogram::ID id) const
    {
      auto const file_info (puzzles_.at (id).puzzle);
      if (!file_info.exists())
      {
        QMessageBox::critical
          ( nullptr
          , "File not found!"
          , QString ("Puzzle file %1% not found!").arg (id.puzzle.name)
          );
      }

      QFile file (file_info.filePath());
      file.open (QIODevice::ReadOnly);

      QTextStream in (&file);

      QString const dimensions_line (in.readLine());
      QStringList const dimensions (dimensions_line.split (" "));

      in.skipWhiteSpace();

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

      if (hasAnswer (id))
      {
        return { id
               , std::move (data::Solution (data))
               , answer (id)
               };
      }
      else
      {
        return {id, std::move (data::Solution (data))};
      }
    }
  }
}
