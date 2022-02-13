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
      static data::Nonogram::Pack internalPack();

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

      using Files = std::map<data::Nonogram::ID, Info>;

      Puzzles();

      data::Nonogram::Packs packs() const;
      data::Nonogram::Puzzles puzzlesOfPack (data::Nonogram::Pack) const;

      bool hasAnswer (data::Nonogram::ID) const;
      data::Answer answer (data::Nonogram::ID) const;
      void writeAnswer (data::Nonogram::ID, data::Nonogram const&);

      std::optional<data::Nonogram::ID> currentPuzzle() const;
      void setCurrentPuzzle (data::Nonogram::ID);
      bool hasBeenSolved (data::Nonogram::Pack) const;
      bool hasBeenSolved (data::Nonogram::ID) const;
      void setSolved (data::Nonogram::ID);
      data::Nonogram puzzle (data::Nonogram::ID) const;

      data::Nonogram titleNonogram() const;
      data::Nonogram tutorialNonogram() const;

    private:
      QString const root_path_;
      QString const answers_path_;
      QString const puzzles_path_;
      Files puzzles_;
    };
  }
}
