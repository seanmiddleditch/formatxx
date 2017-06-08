// formatxx - C++ string formatting library.
//
// This is free and unencumbered software released into the public domain.
// 
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non - commercial, and by any
// means.
// 
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
// 
// For more information, please refer to <http://unlicense.org/>
//
// Authors:
//   Sean Middleditch <sean@middleditch.us>

#include <formatxx/format.h>
#include <formatxx/wide.h>

#include <formatxx/_detail/format_traits.h>
#include <formatxx/_detail/parse_unsigned.h>
#include <formatxx/_detail/parse_format.h>
#include <formatxx/_detail/write_integer.h>
#include <formatxx/_detail/format_impl.h>
#include <formatxx/_detail/printf_impl.h>

#include <cstdio>
#include <limits>
#include <climits>
#include <cinttypes>

namespace formatxx {

FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, char ch, string_view)
{
	out.write(string_view(&ch, 1));
}

FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, bool value, string_view spec)
{
	format_value(out, value ? _detail::FormatTraits<char>::sTrue : _detail::FormatTraits<char>::sFalse, spec);
}

FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, char* zstr, string_view spec)
{
	format_value(out, string_view(zstr), spec);
}

FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, char const* zstr, string_view spec)
{
	format_value(out, string_view(zstr), spec);
}

FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, string_view str, string_view)
{
	out.write(str);
}

FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, signed int value, string_view spec) { _detail::write_integer(out, value, spec); }
FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, signed char value, string_view spec) { _detail::write_integer(out, value, spec); }
FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, signed long value, string_view spec) { _detail::write_integer(out, value, spec); }
FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, signed short value, string_view spec) { _detail::write_integer(out, value, spec); }
FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, signed long long value, string_view spec) { _detail::write_integer(out, value, spec); }

FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, unsigned int value, string_view spec) { _detail::write_integer(out, value, spec); }
FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, unsigned char value, string_view spec) { _detail::write_integer(out, value, spec); }
FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, unsigned long value, string_view spec) { _detail::write_integer(out, value, spec); }
FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, unsigned short value, string_view spec) { _detail::write_integer(out, value, spec); }
FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, unsigned long long value, string_view spec) { _detail::write_integer(out, value, spec); }

FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, float value, string_view spec)
{
	format_value(out, static_cast<double>(value), spec);
}

FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, double value, string_view spec_string)
{
	char fmt[3] = "%f";

	format_spec const spec = parse_format_spec(spec_string);

	switch (spec.code)
	{
	case 'a':
	case 'A':
	case 'e':
	case 'E':
	case 'f':
	case 'F':
	case 'g':
	case 'G':
		fmt[1] = spec.code;
		break;
	default:
		// leave default
		break;
	}

	char buf[1048]; // not actually enough for every float, but...
	int len = std::snprintf(buf, sizeof(buf), fmt, value);
	if (len > 0)
		out.write(string_view(buf, len));
}

FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, void* ptr, string_view spec)
{
	_detail::write_integer(out, reinterpret_cast<std::uintptr_t>(ptr), spec);
}

FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, void const* ptr, string_view spec)
{
	_detail::write_integer(out, reinterpret_cast<std::uintptr_t>(ptr), spec);
}

namespace _detail {

template FORMATXX_PUBLIC basic_format_writer<char>& FORMATXX_API format_impl(basic_format_writer<char>& out, basic_string_view<char> format, std::size_t count, BasicFormatterThunk<char> const* funcs, FormatterParameter const* values);

template FORMATXX_PUBLIC basic_format_writer<char>& FORMATXX_API printf_impl(basic_format_writer<char>& out, basic_string_view<char> format, std::size_t count, BasicFormatterThunk<char> const* funcs, FormatterParameter const* values);

} // namespace _detail

template FORMATXX_PUBLIC basic_format_spec<char> FORMATXX_API parse_format_spec(basic_string_view<char>);
template FORMATXX_PUBLIC basic_format_spec<wchar_t> FORMATXX_API parse_format_spec(basic_string_view<wchar_t>);

} // namespace formatxx