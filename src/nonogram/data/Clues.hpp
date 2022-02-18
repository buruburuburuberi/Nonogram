#pragma once

#include <nonogram/data/clues/Data.hpp>
#include <nonogram/data/grid/Cell.hpp>
#include <nonogram/data/grid/Data.hpp>

#include <array>
#include <cstddef>

namespace nonogram
{
  namespace data
  {
    class Clues
    {
    public:
      using Value = std::size_t;
      using Data = clues::Data<Value>;

      enum class Type
      { Column
      , Row
      };

      static std::array<Type, 2> constexpr all_types
      { Type::Column
      , Type::Row
      };

      static clues::FullIndex toFullIndex (Type, grid::Cell);

      Clues (grid::Data<bool> const&, Type);

      clues::MinorIndex maxNumberOfClues() const;
      clues::MainIndex mainSize() const;
      clues::MinorIndex minorSize (clues::MainIndex) const;

      Value clue (clues::FullIndex) const;

    private:
      Data computeClues (grid::Data<bool> const&, Type);

      Type type_;
      clues::MinorIndex max_minor_count_;
      Data data_;
    };
  }
}
