#pragma once

#include <nonogram/data/Answer.hpp>
#include <nonogram/data/Nonogram.hpp>

#include <QtCore/QDir>
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
      Puzzles();

      data::Nonogram::Packs packs() const;
      data::Nonogram::Puzzles puzzlesOfPack (data::Nonogram::Pack) const;

      bool hasAnswer (data::Nonogram::ID) const;
      data::Answer answer (data::Nonogram::ID) const;
      void writeAnswer (data::Nonogram::ID, data::Nonogram const&);

      std::optional<data::Nonogram::ID> currentPuzzle() const;
      bool hasBeenSolved (data::Nonogram::Pack) const;
      bool hasBeenSolved (data::Nonogram::ID) const;
      void markAsSolved (data::Nonogram::ID);
      data::Nonogram puzzle (data::Nonogram::ID) const;

      data::Nonogram titleNonogram() const;
      data::Nonogram tutorialNonogram() const;

    private:
      QString solvedKey (data::Nonogram::ID) const;

      QString const current_puzzle_key_;
      QDir const root_dir_;
      QDir const answers_dir_;
      QDir const puzzles_dir_;

      struct Info
      {
        Info (QFileInfo _puzzle);

        QFileInfo puzzle;
        std::optional<QFileInfo> answer;
      };

      std::map<data::Nonogram::ID, Info> puzzles_;
    };
  }
}
