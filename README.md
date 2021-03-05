# EnumStrings

A tiny single-header C++14 utility that helps automate conversion between enumerations and strings.

Compared to full-featured libraries like [Better Enums](http://aantron.github.io/better-enums/) and [Magic Enum C++](https://github.com/Neargye/magic_enum),
this is a much simpler and more lightweight utility. 
In particular, it **does not automate** generation of strings associated with enumeration values: the user must provide them explicitly.
It's a disadvantage for most users, but can be desirable for some: 
for example, when your project's coding conventions require enum values to be UPPER_CASE, but you want user inputs to be lower case;
or when you want user-facing strings to contain characters not allowed in C++ identifiers (like spaces).

On the plus side, `EnumStrings` 
* **does not** rely on compiler-specific hacks (and so works with any C++14 compiler);
* **does not** involve a large amount of code generation (the macro invoked is *really* small);
* **does not** wrap the enum in some wrapper type 

In summary, I **strongly** suggest you check out the two libraries mentioned above first, and only come back here if neither of them works for you.
Also, please read the section below.

Limitations
-----------

* Enumeration values must start from 0 and be contiguous.
  In other words, only default compiler-assigned values should be used.
  This works fine for the intended use-case: converting user inputs into integral constants that represent options/alternatives that are switched upon.
  This does not work if underlying numerical values have a meaning of their own and must be custom.
* There is no check that the number of strings provided in the macro call matches the number of user-defined enumeration constants.
  The user is responsible for keeping the macro invocation up-to-date with enum definition.

Usage
-----

There is no installation process - just copy the file `enum_strings.h` into your project and use.
Support/versioning are not planned at this time (I will fix bugs and accept pull requests though).

The header provides a single macro `ENUM_STRINGS` that must be called in the same namespace the enumeration is defiend in.
For enums nested in classes, the macro should be called at the namespace scope after the class definition.
The macro takes the enum type and a list of raw string literals that will be associated with contiguous enumeration values.

```c++
#include "EnumStrings.h"

enum WeakEnum { A, B };
ENUM_STRINGS(WeakEnum, "wa", "wb");

enum class StrongEnum : int16_t { A, B };
ENUM_STRINGS(StrongEnum, "sa", "sb");

struct Foo
{
  enum class NestedEnum { A, B, C };
};
ENUM_STRINGS(Foo::NestedEnum, "fa", "fb", "fc");
```

After that, use as follows:
```c++
// convert string to enum
auto const e1 = enum_strings::from_string<WeakEnum>("wb"); // e1 == WeakEnum::B;
auto const e2 = enum_strings::from_string<StrongEnum>("sa"); // e1 == StrongEnum::A;
auto const e3 = enum_strings::from_string<Foo::NestedEnum>("fc"); // e3 == Foo::NestedEnum::C;

// or read from an input stream
StrongEnum e4;
try
{
  std::cin >> e4;
}
catch (std::invalid_argument const & err)
{
  std::cout << err.what() << std::endl; 
}

// convert enum to string
auto const s1 = enum_strings::to_string(WeakEnum::A); // s1 == "wa"
auto const s2 = enum_strings::to_string(StrongEnum::B); // s2 == "sb"
auto const s3 = enum_strings::to_string(Foo::NestedEnum::B); // s3 == "fc"

// or print to an output stream
StrongEnum const e5 = StrongEnum::A;
std::cout << e5; // prints "sa"

// get number of registered string values
auto constexpr n = enum_strings::num_values<Foo::NestedEnum>(); // n == 3

// get a list of registered enum strings (for example, to present valid choices to the user)
auto choices = enum_strings::get_strings<Foo::NestedEnum>(); // std::vector<std::string>{ "fa", "fb", "fc" }
```

Design
------

The utility works by injecting three things into the user namespace:
* an inline function that stores user-provided strings in a local static array;
* an `operator>>` for given enum type;
* an `operator<<` for given enum type.

All three functions are located via argument-dependent lookup (ADL) when used.
Streaming operators are meant to be used by the user, while the first function is intended 
to be called by other utility functions in the `enum_strings` namespace.

The use of local static for storage avoids all kinds of linking problems, but prevents the whole
utility from being `constexpr`. In author's view, compile-time enum-string conversions
are of secondary interest compared to primary intended use (converting runtime input values).
