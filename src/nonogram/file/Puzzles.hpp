#pragma once

#include <nonogram/data/Nonogram.hpp>

#include <QtCore/QFileInfo>
#include <QtCore/QString>

#include <map>

namespace nonogram
{
  namespace file
  {
    class Puzzles
    {
    public:
      using Files = std::map<QString, QFileInfo>;
      using Directories = std::map<QString, Files>;

      Puzzles (QString root_path);

      QStringList packs() const;
      QStringList puzzlesOfPack (QString) const;

      data::Nonogram puzzle (QString pack, QString puzzle) const;

      data::Nonogram titleNonogram() const;

    private:
      Directories puzzles_;
    };
  }
}
