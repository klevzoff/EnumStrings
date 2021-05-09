#ifndef ENUM_STRINGS_H
#define ENUM_STRINGS_H

/**
 * @file enum_strings.h
 * @author Sergey Klevtsov
 */

#include <string>
#include <vector>
#include <type_traits>
#include <stdexcept>
#include <utility>

/**
 * @brief Associate a list of string names with enumeration values.
 * @param ENUM the enumeration type
 * @param ... list of names (C-string literals)
 *
 * Conditions (not enforced but won't work correctly if violated):
 *  - the macro must be called at namespace scope the enumeration type is defined in
 *  - the number and order of string arguments passed must match the enum values
 *  - enumeration constants must not have custom values assigned
 *
 *  If the enumeration has a special constant named @p END, it should be
 *  the last one, and its value will be used to determine the number
 *  of enum values and checked at compile time against number of strings.
 */
#define ENUM_STRINGS(E, ...)                                    \
  static_assert(std::is_enum<E>::value,                         \
                "Not an enumeration type");                     \
                                                                \
  inline auto const & _get_enum_strings(E)                      \
  {                                                             \
    static constexpr char const * ss[] { __VA_ARGS__ };         \
    return ss;                                                  \
  }                                                             \
                                                                \
  inline std::ostream &                                         \
  operator<<(std::ostream & os, E const e)                      \
  {                                                             \
    os << ::enum_strings::to_string(e);                         \
    return os;                                                  \
  }                                                             \
                                                                \
  inline std::istream &                                         \
  operator>>(std::istream & is, E & e)                          \
  {                                                             \
    std::string s; is >> s;                                     \
    e = ::enum_strings::from_string<E>(s);                      \
    return is;                                                  \
  }                                                             \
                                                                \
  static_assert(::enum_strings::detail::check_num_values<E>(0), \
                "Number of strings doesn't match number of enum"\
                " values as determined by END enumeration value")

namespace enum_strings
{

namespace detail
{

template<typename E>
inline auto const & get_strings()
{
  static_assert(std::is_enum<E>::value, "Not an enumeration type");
  return _get_enum_strings(E{}); // invoke ADL
}

template <typename E, typename Container, std::size_t ... Indices>
inline Container get_strings(std::index_sequence<Indices...>)
{
  auto const & strings = ::enum_strings::detail::get_strings<E>();
  return { strings[Indices] ... };
}

template <typename T>
struct size;

template <typename T, std::size_t N>
struct size<T[N]>
{
  static constexpr std::size_t value = N;
};

} // namespace detail

/**
 * @brief Get the number of enum values that are associated with strings.
 * @tparam E type of enumeration
 * @return the number of values
 */
template <typename E>
inline constexpr std::size_t num_values()
{
  using arr_type = std::remove_const_t<std::remove_reference_t<decltype(::enum_strings::detail::get_strings<E>())>>;
  return ::enum_strings::detail::size<arr_type>::value;
}

/**
 * @brief Convert enum to string.
 * @tparam E type of enumeration
 * @param e the enum value to convert
 * @return the corresponding string
 * @exception std::invalid_argument if numerical value of @p e is greater of equal than the number of strings.
 */
template<typename E>
inline std::string to_string(E const e)
{
  using base_type = std::underlying_type_t<E>;
  auto const & strings = ::enum_strings::detail::get_strings<E>();
  auto const index = static_cast<base_type>(e);
  if (index >= static_cast<base_type>(::enum_strings::num_values<E>()))
  {
    throw std::invalid_argument("Invalid value " + std::to_string(index) + ". "
                                "Valid range is 0.." + std::to_string(::enum_strings::num_values<E>() - 1));
  }
  return strings[index];
}

/**
 * @brief Convert string to enum
 * @tparam E type of enumeration
 * @param s the string to convert
 * @return the corresponding enum value
 * @exception std::invalid_argument if @p s is not a string associated with give enum type
 */
template <typename E>
inline E from_string(std::string const & s)
{
  auto const & strings = ::enum_strings::detail::get_strings<E>();
  std::size_t n = 0;
  for (; n < ::enum_strings::num_values<E>() && strings[n] != s; ++n);
  if (n == ::enum_strings::num_values<E>())
  {
    throw std::invalid_argument("'" + s + "' is not a valid string representation of this type");
  }
  auto const e = static_cast<E>(n);
  return e;
}

/**
 * @brief Get the enumeration strings as a vector
 * @tparam E type of enumeration
 * @return a vector of <tt>std::string</tt>'s associated with @p E's values
 */
template <typename E>
inline std::vector<std::string> get_strings()
{
  return ::enum_strings::detail::get_strings<E, std::vector<std::string>>(std::make_index_sequence<::enum_strings::num_values<E>()>{});
}

namespace detail
{

template< typename E, E = E::END >
inline constexpr bool check_num_values(int)
{
  return ::enum_strings::num_values<E>() == static_cast<std::size_t>(static_cast<std::underlying_type_t<E>>(E::END));
}

template< typename >
inline constexpr bool check_num_values(...)
{
  return true;
}

} // namespace detail

} //namespace enum_strings

#endif //ENUM_STRINGS_H
