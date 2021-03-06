#pragma once

#include <optional>

namespace nonogram::util
{
  template<typename T>
    auto optional_if (bool condition, T value) -> std::optional<T>
  {
    return condition ? std::optional (value) : std::nullopt;
  }
}
