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

#if !defined(_guard_FORMATXX_WIDE_H)
#define _guard_FORMATXX_WIDE_H
#pragma once

#include <formatxx/format.h>

namespace formatxx {
	namespace _detail { template <typename T> struct new_delete_allocator; }
	template <typename CharT, std::size_t> class basic_fixed_writer;
	template <typename StringT> class basic_string_writer;
	template <typename CharT, std::size_t Size, typename AllocatorT> class basic_buffered_writer;

	using wstring_view = basic_string_view<wchar_t>;
	using wformat_writer = basic_format_writer<wchar_t>;
	using wformat_spec = basic_format_spec<wchar_t>;
}

namespace formatxx {
	/// Default format helpers.
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, wchar_t const* zstr, wstring_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, wchar_t* zstr, wstring_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, wstring_view str, wstring_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, wchar_t ch, wstring_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, bool value, wstring_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, float value, wstring_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, double value, wstring_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, signed char value, wstring_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, signed int value, wstring_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, signed long value, wstring_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, signed short value, wstring_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, signed long long value, wstring_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, unsigned char value, wstring_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, unsigned int value, wstring_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, unsigned long value, wstring_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, unsigned short value, wstring_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, unsigned long long value, wstring_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, void* value, wstring_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, void const* value, wstring_view spec) noexcept;

	/// Format narrow characters into wide writers
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, char const* zstr, wstring_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, char* zstr, wstring_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, string_view str, wstring_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, char ch, wstring_view spec) noexcept;

	/// Format wide characters into narrow writers
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, wchar_t const* zstr, string_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, wchar_t* zstr, string_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, wstring_view str, string_view spec) noexcept;
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, wchar_t ch, string_view spec) noexcept;
}

extern template FORMATXX_PUBLIC formatxx::result_code FORMATXX_API formatxx::_detail::format_impl(basic_format_writer<wchar_t>& out, basic_string_view<wchar_t> format, basic_format_args<wchar_t> args);
extern template FORMATXX_PUBLIC formatxx::result_code FORMATXX_API formatxx::_detail::printf_impl(basic_format_writer<wchar_t>& out, basic_string_view<wchar_t> format, basic_format_args<wchar_t> args);
extern template FORMATXX_PUBLIC formatxx::basic_format_spec<wchar_t> FORMATXX_API formatxx::parse_format_spec(basic_string_view<wchar_t> spec) noexcept;

#endif // !defined(_guard_FORMATXX_WIDE_H)
