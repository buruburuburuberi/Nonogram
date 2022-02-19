#include <QtCore/QString>

namespace nonogram::util
{
#define HARD_QSTRING_TYPEDEF_IMPL(name_)                              \
  struct name_                                                        \
  {                                                                   \
  public:                                                             \
    QString name;                                                     \
                                                                      \
    explicit name_ (QString v)                                        \
    : name (v)                                                        \
    {}                                                                \
                                                                      \
    name_ (name_ const&) = default;                                   \
    name_ (name_&&) = default;                                        \
    name_& operator= (name_ const&) = default;                        \
    name_& operator= (name_&&) = default;                             \
    name_() = default;                                                \
    ~name_() = default;                                               \
                                                                      \
    bool operator!= (name_ const& rhs) const                          \
    { return name != rhs.name; }                                      \
    bool operator<  (name_ const& rhs) const                          \
    { return name < rhs.name; }                                       \
    bool operator<= (name_ const& rhs) const                          \
    { return name <= rhs.name; }                                      \
    bool operator== (name_ const& rhs) const                          \
    { return name == rhs.name; }                                      \
    bool operator>  (name_ const& rhs) const                          \
    { return name > rhs.name; }                                       \
    bool operator>= (name_ const& rhs) const                          \
    { return name >= rhs.name; }                                      \
  };
}
