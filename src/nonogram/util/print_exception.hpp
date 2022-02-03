#pragma once

#include <exception>
#include <ostream>

namespace nonogram
{
  namespace util
  {
    class exception_printer
    {
    public:
      exception_printer (std::exception_ptr exception);

      friend std::ostream& operator<< (std::ostream&, exception_printer const&);

    private:
      std::exception_ptr exception_;
    };
  }
}
