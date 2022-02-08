#pragma once

#include <nonogram/data/Nonogram.hpp>

#include <QtCore/QFileInfo>
#include <QtCore/QString>

#include <map>
#include <optional>

namespace nonogram
{
  namespace file
  {
    class Puzzles
    {
    public:
      static QString internalPackName();

      struct Info
      {
        Info (QFileInfo _puzzle)
        : puzzle (_puzzle)
        {}

        Info (QFileInfo _puzzle, QFileInfo _answer)
        : puzzle (_puzzle)
        , answer (_answer)
        {}

        QFileInfo puzzle;
        std::optional<QFileInfo> answer;
      };

      using Files = std::map<QString, Info>;
      using Directories = std::map<QString, Files>;

      Puzzles();

      QStringList packs() const;
      QStringList puzzlesOfPack (QString) const;

      bool hasAnswer (QString pack, QString puzzle) const;
      data::Answer answer (QString pack, QString puzzle) const;
      void writeAnswer (QString pack, QString puzzle, data::Nonogram const&);

      data::Nonogram puzzle (QString pack, QString puzzle) const;

      data::Nonogram titleNonogram() const;

    private:
      QString const root_path_;
      QString const answers_path_;
      QString const puzzles_path_;
      Directories puzzles_;
    };
  }
}
