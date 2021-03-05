#include "enum_strings.h"

#include <sstream>
#include <cassert>

template <typename E>
void test_to_from_string(E const e, std::string const s)
{
  assert(enum_strings::to_string(e) == s);
  assert(enum_strings::from_string<E>(s) == e);
}

template <typename E>
void test_stream_io(E const e)
{
  std::stringstream ss;
  ss << e;
  E v;
  ss >> v;
  assert(v == e);
}

template <typename E, typename ... ARGS>
void test_get_strings(ARGS && ... args)
{
  std::vector<std::string> expected { args ... };
  assert(::enum_strings::get_strings<E>() == expected);
}

///////////////////////////////

enum WeakEnum { A, B };
ENUM_STRINGS(WeakEnum, "wa", "wb");

enum class StrongEnum : int16_t { A, B };
ENUM_STRINGS(StrongEnum, "sa", "sb");

struct Foo
{
  enum class NestedEnum { A, B };
};
ENUM_STRINGS(Foo::NestedEnum, "fa", "fb");

///////////////////////////////

int main()
{
  test_to_from_string(A, "wa");
  test_to_from_string(B, "wb");
  test_stream_io(A);
  test_stream_io(B);
  test_get_strings<WeakEnum>("wa", "wb");

  test_to_from_string(StrongEnum::A, "sa");
  test_to_from_string(StrongEnum::B, "sb");
  test_stream_io(StrongEnum::A);
  test_stream_io(StrongEnum::B);
  test_get_strings<StrongEnum>("sa", "sb");

  test_to_from_string(Foo::NestedEnum::A, "fa");
  test_to_from_string(Foo::NestedEnum::B, "fb");
  test_stream_io(Foo::NestedEnum::A);
  test_stream_io(Foo::NestedEnum::B);
  test_get_strings<Foo::NestedEnum>("fa", "fb");

  return 0;
}
