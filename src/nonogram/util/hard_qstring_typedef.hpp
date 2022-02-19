#pragma once

namespace nonogram::util
{
  //! `HARD_QSTRING_TYPEDEF (T);` provides a type `T` with
  //!
  //! * typename T::underlying_type = QString
  //! * explicit ctor from QString
  //! * defaulted other ctors
  //!
  //! * !=, <, <=, ==, >, >=
  //!
#define HARD_QSTRING_TYPEDEF(name_)                    \
  HARD_QSTRING_TYPEDEF_IMPL (name_)
}

#include <nonogram/util/hard_qstring_typedef.ipp>
