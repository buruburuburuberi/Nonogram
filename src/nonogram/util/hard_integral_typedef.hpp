#pragma once

namespace nonogram
{
  namespace util
  {
    //! `HARD_INTEGRAL_TYPEDEF (T, integral-type);` provides
    //! a type `T` with
    //!
    //! * typename T::underlying_type = integral-type
    //! * explicit ctor from integral-type
    //! * defaulted other ctors
    //!
    //! * !=, <, <=, ==, >, >=
    //! * ++t, t++
    //! * +, -, *, /, %
    //! * .toString()
    //!
#define HARD_INTEGRAL_TYPEDEF(name_, base_)                    \
    HARD_INTEGRAL_TYPEDEF_IMPL (name_, base_)
  }
}

#include <nonogram/util/hard_integral_typedef.ipp>
