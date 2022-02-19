#include <nonogram/file/Puzzles.hpp>

#include <nonogram/data/Solution.hpp>
#include <nonogram/data/clues/Data.hpp>
#include <nonogram/data/grid/Data.hpp>
#include <nonogram/util/optional_if.hpp>

#include <QtCore/QCoreApplication>
#include <QtCore/QDataStream>
#include <QtCore/QDirIterator>
#include <QtCore/QSettings>
#include <QtCore/QTextStream>
#include <QtWidgets/QMessageBox>

#include <algorithm>

namespace nonogram
{
  namespace file
  {
    Puzzles::Info::Info (QFileInfo _puzzle)
    : puzzle (_puzzle)
    {}

    Puzzles::Puzzles()
    : current_puzzle_key_ ("current")
    , root_dir_ (QCoreApplication::applicationDirPath())
    , answers_dir_ (root_dir_.path() + "/../answers")
    , puzzles_dir_ (root_dir_.path() + "/../puzzles")
    , puzzles_()
    {
      QDirIterator puzzles_directory_it
        (puzzles_dir_.path(), QDir::AllDirs | QDir::NoDotAndDotDot);
      while (puzzles_directory_it.hasNext())
      {
        QDir const directory (puzzles_directory_it.next());
        data::Nonogram::Pack const pack {directory.dirName()};

        QDirIterator file_it (directory.path(), QDir::Files);
        while (file_it.hasNext())
        {
          if ( QFileInfo const file (file_it.next());
               file.suffix() == "nonogram"
             )
          {
            puzzles_.emplace
              ( std::piecewise_construct
              , std::forward_as_tuple ( pack
                                      , data::Nonogram::Puzzle {file.baseName()}
                                      )
              , std::forward_as_tuple (file)
              );
          }
        }
      }

      QDirIterator answers_directory_it
        (answers_dir_.path(), QDir::AllDirs | QDir::NoDotAndDotDot);
      while (answers_directory_it.hasNext())
      {
        QDir const directory (answers_directory_it.next());
        data::Nonogram::Pack const pack {directory.dirName()};

        if ( std::none_of
              ( puzzles_.begin()
              , puzzles_.end()
              , [pack] (auto puzzle) { return puzzle.first.pack == pack; }
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
            throw std::runtime_error
              ( QString ("Puzzle %1 not found.")
                  .arg (id.toString()).toStdString()
              );
          }

          puzzles_.at (id).answer = file;
        }
      }
    }

    QString Puzzles::solvedKey (data::Nonogram::ID id) const
    {
      return QString ("solved-%1").arg (id.toString());
    }

    data::Nonogram Puzzles::titleNonogram() const
    {
      return
        { {data::Nonogram::internalPack, data::Nonogram::Puzzle {"Title"}}
        , data::grid::Data<bool>
          { data::grid::Column (25)
          , data::grid::Row (13)
          , { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            , 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0
            , 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0
            , 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0
            , 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0
            , 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0
            , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            , 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0
            , 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0
            , 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0
            , 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0
            , 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0
            , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            }
          }
        , {}
        };
    }

    data::Nonogram Puzzles::tutorialNonogram() const
    {
      return { { data::Nonogram::internalPack
               , data::Nonogram::Puzzle {"Tutorial"}
               }
             , data::grid::Data<bool>
                 { data::grid::Column (5)
                 , data::grid::Row (5)
                 , { 0, 0, 1, 0, 0
                   , 0, 1, 1, 1, 0
                   , 1, 1, 0, 1, 1
                   , 0, 1, 1, 1, 0
                   , 0, 0, 1, 0, 0
                   }
                 }
             , {}
             };
    }

    data::Nonogram::Packs Puzzles::packs() const
    {
      data::Nonogram::Packs packs;

      for (auto const& [id, info] : puzzles_)
      {
        packs.insert (id.pack);
      }

      return packs;
    }

    data::Nonogram::Puzzles Puzzles::puzzlesOfPack (data::Nonogram::Pack pack) const
    {
      data::Nonogram::Puzzles puzzles;

      for (auto const& [id, info] : puzzles_)
      {
        if (id.pack == pack)
        {
          puzzles.insert (id.puzzle);
        }
      }

      return puzzles;
    }

    bool Puzzles::hasAnswer (data::Nonogram::ID id) const
    {
      return puzzles_.at (id).answer.has_value();
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

      QDataStream in (&file);

      data::Answer answer (in);

      file.close();

      return answer;
    }

    void Puzzles::writeAnswer ( data::Nonogram::ID id
                              , data::Nonogram const& nonogram
                              )
    {
      auto const pack_name (id.pack.name);
      auto const puzzle_name (id.puzzle.name);

      QDir directory (answers_dir_);
      if (!directory.exists (pack_name))
      {
        directory.mkdir (pack_name);
      }
      directory.cd (pack_name);

      QFile file (directory.absoluteFilePath (puzzle_name));
      file.open (QIODevice::WriteOnly);

      puzzles_.at (id).answer = file;

      QDataStream out (&file);

      out << nonogram.answer();

      file.close();

      QSettings settings;
      settings.setValue (current_puzzle_key_, id.toString());
    }

    std::optional<data::Nonogram::ID> Puzzles::currentPuzzle() const
    {
      QSettings settings;
      return util::optional_if
          ( settings.contains (current_puzzle_key_)
          , data::Nonogram::ID (settings.value (current_puzzle_key_).toString())
          );

    }

    bool Puzzles::hasBeenSolved (data::Nonogram::Pack pack) const
    {
      auto const puzzles (puzzlesOfPack (pack));

      return std::all_of
          ( puzzles.begin()
          , puzzles.end()
          , [&] (auto puzzle) { return hasBeenSolved ({pack, puzzle}); }
          );
    }

    bool Puzzles::hasBeenSolved (data::Nonogram::ID id) const
    {
      QSettings settings;
      auto const key (solvedKey (id));
      return settings.contains (key) && settings.value (key).toBool();
    }

    void Puzzles::markAsSolved (data::Nonogram::ID id)
    {
      QSettings settings;
      settings.setValue (solvedKey (id), true);
    }

    data::Nonogram Puzzles::puzzle (data::Nonogram::ID id) const
    {
      auto const file_info (puzzles_.at (id).puzzle);
      if (!file_info.exists())
      {
        QMessageBox::critical
          ( nullptr
          , "File not found!"
          , QString ("Puzzle file %1 from pack %2 not found!")
              .arg (id.puzzle.name).arg (id.pack.name)
          );
      }

      QFile file (file_info.filePath());
      file.open (QIODevice::ReadOnly);

      QTextStream in (&file);

      QString const dimensions_line (in.readLine());
      QStringList const dimensions (dimensions_line.split (" "));

      in.skipWhiteSpace();

      data::grid::Data<bool> data
        ( data::grid::Column {dimensions.at (0).toUInt()}
        , data::grid::Row {dimensions.at (1).toUInt()}
        );

      data::grid::Row row {0};
      while (!in.atEnd())
      {
        QString const line (in.readLine());
        if (line.isEmpty())
        {
          continue;
        }

        QStringList const datum_list (line.split (" ", QString::SkipEmptyParts));

        data::grid::Column column {0};
        for (auto const& datum : datum_list)
        {
          data.set (column, row, datum == "#");

          ++column;
        }
        ++row;
      }

      file.close();

      return { id
             , data::Solution (data)
             , util::optional_if (hasAnswer (id), answer (id))
             };
    }
  }
}
