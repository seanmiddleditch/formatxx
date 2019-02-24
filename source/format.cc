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
#include <formatxx/_detail/write_string.h>
#include <formatxx/_detail/write_float.h>
#include <formatxx/_detail/format_impl.h>
#include <formatxx/_detail/printf_impl.h>

#include <cstdio>

namespace formatxx {

	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, char value, string_view spec) noexcept { _detail::write_char(out, value, spec); }
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, char const* value, string_view spec) noexcept { _detail::write_string<char>(out, value, spec); }
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, char* value, string_view spec) noexcept { _detail::write_string<char>(out, value, spec); }
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, string_view value, string_view spec) noexcept { _detail::write_string<char>(out, value, spec); }

	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, signed int value, string_view spec) noexcept { _detail::write_integer(out, value, spec); }
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, signed char value, string_view spec) noexcept { _detail::write_integer(out, value, spec); }
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, signed long value, string_view spec) noexcept { _detail::write_integer(out, value, spec); }
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, signed short value, string_view spec) noexcept { _detail::write_integer(out, value, spec); }
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, signed long long value, string_view spec) noexcept { _detail::write_integer(out, value, spec); }

	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, unsigned int value, string_view spec) noexcept { _detail::write_integer(out, value, spec); }
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, unsigned char value, string_view spec) noexcept { _detail::write_integer(out, value, spec); }
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, unsigned long value, string_view spec) noexcept { _detail::write_integer(out, value, spec); }
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, unsigned short value, string_view spec) noexcept { _detail::write_integer(out, value, spec); }
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, unsigned long long value, string_view spec) noexcept { _detail::write_integer(out, value, spec); }

	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, bool value, string_view spec) noexcept
	{
		format_value(out, value ? _detail::FormatTraits<char>::sTrue : _detail::FormatTraits<char>::sFalse, spec);
	}


	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, float value, string_view spec) noexcept { _detail::write_float(out, value, spec); }
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, double value, string_view spec) noexcept { _detail::write_float(out, value, spec); }

	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, void* ptr, string_view spec) noexcept { _detail::write_integer(out, reinterpret_cast<std::uintptr_t>(ptr), spec); }
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, void const* ptr, string_view spec) noexcept { _detail::write_integer(out, reinterpret_cast<std::uintptr_t>(ptr), spec); }

	template FORMATXX_PUBLIC result_code FORMATXX_API _detail::format_impl(basic_format_writer<char>& out, basic_string_view<char> format, basic_format_args<char> args);
	template FORMATXX_PUBLIC result_code FORMATXX_API _detail::printf_impl(basic_format_writer<char>& out, basic_string_view<char> format, basic_format_args<char> args);
	template FORMATXX_PUBLIC basic_format_spec<char> FORMATXX_API parse_format_spec(basic_string_view<char>) noexcept;
} // namespace formatxx