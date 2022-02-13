#include <string>
#include <type_traits>

namespace nonogram
{
  namespace util
  {
#define HARD_INTEGRAL_TYPEDEF_IMPL(name_, base_)                        \
    struct name_                                                        \
    {                                                                   \
      using underlying_type = base_;                                    \
                                                                        \
    public:                                                             \
      underlying_type value {0};                                        \
                                                                        \
      explicit constexpr name_ (decltype (value) v) : value (v) {}      \
                                                                        \
      name_ (name_ const&) = default;                                   \
      name_ (name_&&) = default;                                        \
      name_& operator= (name_ const&) = default;                        \
      name_& operator= (name_&&) = default;                             \
      name_() = default;                                                \
      ~name_() = default;                                               \
                                                                        \
      name_& operator++() { value++; return *this; }                    \
      name_ operator++ (int) { auto r (*this); ++value; return r; }     \
                                                                        \
      name_ operator+ (name_ const& rhs) const                          \
      { return name_ (value + rhs.value); }                             \
      name_ operator- (name_ const& rhs) const                          \
      { return name_ (value - rhs.value); }                             \
      name_ operator* (name_ const& rhs) const                          \
      { return name_ (value * rhs.value); }                             \
      name_ operator/ (name_ const& rhs) const                          \
      { return name_ (value / rhs.value); }                             \
      name_ operator% (name_ const& rhs) const                          \
      { return name_ (value % rhs.value); }                             \
                                                                        \
      constexpr bool operator!= (name_ const& rhs) const                \
      { return value != rhs.value; }                                    \
      constexpr bool operator<  (name_ const& rhs) const                \
      { return value < rhs.value; }                                     \
      constexpr bool operator<= (name_ const& rhs) const                \
      { return value <= rhs.value; }                                    \
      constexpr bool operator== (name_ const& rhs) const                \
      { return value == rhs.value; }                                    \
      constexpr bool operator>  (name_ const& rhs) const                \
      { return value > rhs.value; }                                     \
      constexpr bool operator>= (name_ const& rhs) const                \
      { return value >= rhs.value; }                                    \
                                                                        \
      std::string toString() const                                      \
      { return std::to_string (value); }                                \
                                                                        \
      static_assert ( std::is_integral<base_>{}                         \
                    , "hard typedef base must be integral"              \
                    );                                                  \
    };
  }
}
