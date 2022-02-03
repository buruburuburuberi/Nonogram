#include <nonogram/util/print_exception.hpp>

namespace nonogram
{
  namespace util
  {
    void print_unknown (std::ostream& os, bool is_first)
    {
      if (!is_first)
      {
        os << std::endl;
      }

      os << "unknown exception type";
    }

    void print_exception ( std::ostream& os
                         , std::exception const& e
                         , bool is_first
                         )
    {
      if (!is_first)
      {
        os << std::endl;
      }

      os << e.what();

      try
      {
        std::rethrow_if_nested (e);
      }
      catch (std::exception const& caught)
      {
        print_exception (os, caught, is_first);
      }
      catch (...)
      {
        print_unknown (os, is_first);
      }
    }

    void print_exception ( std::ostream& os
                         , std::exception_ptr const& e
                         , bool is_first
                         )
    {
      try
      {
        std::rethrow_exception (e);
      }
      catch (std::exception const& caught)
      {
        print_exception (os, caught, is_first);
      }
      catch (...)
      {
        print_unknown (os, is_first);
      }
    }

    exception_printer::exception_printer (std::exception_ptr exception)
    : exception_ (exception)
    {}

    std::ostream& operator<< (std::ostream& os, exception_printer const& printer)
    {
      print_exception (os, printer.exception_, true);

      return os;
    }
  }
}
