# formatxx

C++ string formatting library.

## Authors

Sean Middleditch <sean@middleditch.us>

## About

formatxx is a modern C++ string formatting library. Its intended goals are to offer fast compilation
times, minimal binary bloat, and reasonable speed. Final measurements of these goals are not yet
available; the library is still in very early development.

The library supports writing primitive types as well as user-defined types into string formatting
buffers. The libray has as little dependence on the C++ standard library as possible, which is
intended to make it a very light header to include throughout a larger C++ project.

The included string buffers allow for formatting into a `std::string`-compatible type, a fixed-
size buffer with guaranteed no allocations, or a buffer that initially has a fixed-size buffer
but can grow to accomodate larger strings. The combination of these buffers allow for easy use
in three major cases: quick creation of `std::string` values, use in assert handlers that
cannot allocate, and use in log systems where allocation should be avoided but is allowed when
necessary.

The underlying method of operation of formatxx is to collect a list of arguments via variadic
templates, lookup a `format_value` function for each of those arguments, and then pass the format
string, an array of format functions, and an array of `void` pointers to arguments into the
the actual formatting function. The header mechanisms that generate these lists of functions
and pointers are intended to be as absolutel light-weight on the compiler as possible. The
actual formatting work is all implemented in a source file and not the header, to keep the
header small and cheap to include.

## Usage

formatxx can write into user-defined buffers, or a user may use one of the provided buffer
types. Formatting is support for any type that has an appropriate `format_value` free function with
the signature `void format_value(formatxx::IWriter&, TheType, formatxx::format_spec)`. For instance:

```C++
struct Foo { int value };
	
void format_value(formatxx::writer& out, Foo foo, formatxx::string_view spec)
{
	format(out, "Foo({})", foo.value);
}
	
int main()
{
	std::cout << formatxx::FormatString<>("testing {0}", Foo{123});
}
```

The above will print `testing Foo(123)` to standard output.

The `spec` argument are additional options passed to the formatter. These can be
interpreted by the `format_value` function anyway it sees fit. The
`formatxx::parse_format_spec` function will return a `formatxx::format_spec` structure
with various printf-style flags and options parsed, which are used by default for built-in
format types like integers, floats, and strings.

The `formatxx::format<StringT = std::string>(string_view, ...)` template can be used
for formatting a series of arguments into a `std::string` or any compatible string type.

The `formatxx::format(formatxx::writer&, string_view, ...)` template can be used to
write into a write buffer.

The provided write buffers are:
- `fmt::fixed_writer<N>` - a write buffer that will never allocate but only support
  `N`-1 characters.
- `fmt::string_writer<StringT>` - a write buffer that writes into a `std::string`-
  compatible type.
- `fmt::buffered_writer<N, AllocatorT = std::allocator<char>>` - a write buffer that
  will not allocate for strings up to `N`-1 characters long but will allocate when
  necessary if that length is exceeded.

All three of the provided write buffers guarantee NUL-terminated strings, but support
use with string types that are not NUL-terminated (another important use case for
formatxx).

## History and Design Notes

The library that motivated this author to write formatxx is the excellent
[cppformat](https://cppformat.github.io/) by Victor Zverovich. cppformat does much of what this
library does, has excellent runtime speed, and a responsive and talented author. Unfortunately,
the library failed to meet a few needs that cppformat seeks to address: cppformat relies on
some very heavy standard headers, its error handling mechanisms are limited to C++ exceptions
(which cannot be used in some industries) or `abort`, the compilation time overhead of
including its primary header is pretty large, and its support for formatting user-defined
types relies on `std::ostream` wrappers (which are neither lightweight includes nor are they
runtime efficient). For these reasons, research into formatxx began.

The initial design did not use a `format_value` function for each type. Instead, an enumeration
for the basic categories of primitives (`bool`, `signed long`, `double`, etc.) was computed via
a template, values would be cast into their desired "native" representation, and that would
be passed into the format function. Unfortunately, this added a lot of heavy template machinery
to the header: the enum selection, a `std::tuple` for storing the converted inputs, etc. It
also proved to be difficult to get good support for `format_value` functions for user-defined
types with clean and concise error messages.

The current internal implementation supports a home-grown format lightly modeled after than in
cppformat. The intended syntax will allow positional and non-positional arguments, "standard"
format specifiers (width, precision, alignment, etc.), and custom format specifiers for
user-defined types.

The current header relies on a function template wrapper around the real formatting functions.
This is one template more than is desired that will lead to object file bloat, and for
unoptimized debug builds essentially means that all `format_value` functions get an extra
unnecessary "trampoline" that can be very heavyweight with many compilers' debug checks
enabled. Ideally, these templates would be externed or converted away from wrappers. One
solution would be to make the templates' functions for primitive types directly be the
formatters for those types and then make `format_value` be the template wrapper (since it is
expected to be more rarely used; it mostly exists to keep the same API between primitives and
user-defined types for generic user code).

## To Do

- The remaining primitive types.
- Detectable errors
  - Throw by default, with option/`std::nothrow` to disable?
  - Return value to indicate if an error happened?
- `printf` syntax option.
- Performance pass
  - noexcept(true) where appropriate.
- wchar/u8/u16/u32 string support? maybe.

## Copying

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
