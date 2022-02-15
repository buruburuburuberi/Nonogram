#pragma once

#include <nonogram/data/Array2D.hpp>
#include <nonogram/data/VectorOfVectors.hpp>

#include <QtCore/QSize>

#include <array>
#include <cstddef>
#include <map>

namespace nonogram
{
  namespace data
  {
    class Clues
    {
    public:
      using Value = std::size_t;
      using Data = VectorOfVectors<Value>;

      enum class Type
      { Column
      , Row
      };

      static std::array<Type, 2> constexpr all_types
      { Type::Column
      , Type::Row
      };

      static FullIndex toFullIndex (Type, Slot);

      Clues (Array2D<bool> const&, Type);

      MinorIndex maxNumberOfClues() const;
      MainIndex mainSize() const;
      MinorIndex minorSize (MainIndex) const;

      Value clue (FullIndex) const;

    private:
      Data computeClues (Array2D<bool> const&, Type);

      Type type_;
      MinorIndex max_minor_size_;
      Data data_;
    };
  }
}
