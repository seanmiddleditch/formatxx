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

namespace formatxx
{
	using wstring_view = basic_string_view<wchar_t>;
	using wformat_writer = basic_format_writer<wchar_t>;
	using wformat_spec = basic_format_spec<wchar_t>;

	template <typename... Args> wformat_writer& format(wformat_writer& writer, wstring_view format, Args const&... args);
	template <typename... Args> wformat_writer& printf(wformat_writer& writer, wstring_view format, Args const&... args);

	extern template FORMATXX_PUBLIC wformat_spec FORMATXX_API parse_format_spec(wstring_view spec);
}

namespace formatxx
{
	/// Default format helpers.
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, wchar_t* zstr, wstring_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, wchar_t const* zstr, wstring_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, wstring_view str, wstring_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, wchar_t ch, wstring_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, bool value, wstring_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, float value, wstring_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, double value, wstring_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, signed char value, wstring_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, signed int value, wstring_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, signed long value, wstring_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, signed short value, wstring_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, signed long long value, wstring_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, unsigned char value, wstring_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, unsigned int value, wstring_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, unsigned long value, wstring_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, unsigned short value, wstring_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, unsigned long long value, wstring_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, void* value, wstring_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, void const* value, wstring_view spec);

	/// Formatting for enumerations, using their numeric value.
	template <typename EnumT>
	auto FORMATXX_API format_value(wformat_writer& out, EnumT value, wstring_view spec) -> std::enable_if_t<std::is_enum<EnumT>::value>
	{
		format_value(out, std::underlying_type_t<EnumT>(value), spec);
	}

	template <typename PointerT>
	auto FORMATXX_API format_value(wformat_writer& out, PointerT value, wstring_view spec) -> std::enable_if_t<std::is_pointer<PointerT>::value>
	{
		format_value(out, static_cast<void const*>(value), spec);
	}

	/// Cause a friendlier error message on unknown type.
	template <typename T>
	auto FORMATXX_API format_value(wformat_writer& writer, T const& value, wstring_view spec) -> std::enable_if_t<!std::is_enum<T>::value && !std::is_pointer<T>::value> = delete;

	/// @internal
	namespace _detail
	{
		extern template FORMATXX_PUBLIC basic_format_writer<wchar_t>& FORMATXX_API format_impl(basic_format_writer<wchar_t>& out, basic_string_view<wchar_t> format, std::size_t count, BasicFormatterThunk<wchar_t> const* funcs, FormatterParameter const* values);
		extern template FORMATXX_PUBLIC basic_format_writer<wchar_t>& FORMATXX_API printf_impl(basic_format_writer<wchar_t>& out, basic_string_view<wchar_t> format, std::size_t count, BasicFormatterThunk<wchar_t> const* funcs, FormatterParameter const* values);
	}
}

/// Write the string format using the given parameters into a buffer.
/// @param writer The write buffer that will receive the formatted text.
/// @param format The primary text and formatting controls to be written.
/// @param args The arguments used by the formatting string.
template <typename... Args>
formatxx::wformat_writer& formatxx::format(wformat_writer& writer, wstring_view format, Args const&... args)
{
	_detail::FormatterParameter const values[] = {std::addressof(args)..., nullptr};
	_detail::BasicFormatterThunk<wchar_t> const funcs[] = {&_detail::format_value_thunk<wchar_t, Args>..., nullptr};

	return _detail::format_impl(writer, format, sizeof...(args), funcs, values);
}

/// Write the printf format using the given parameters into a buffer.
/// @param writer The write buffer that will receive the formatted text.
/// @param format The primary text and printf controls to be written.
/// @param args The arguments used by the formatting string.
template <typename... Args>
formatxx::wformat_writer& formatxx::printf(wformat_writer& writer, wstring_view format, Args const&... args)
{
	_detail::FormatterParameter const values[] = {std::addressof(args)..., nullptr};
	_detail::BasicFormatterThunk<wchar_t> const funcs[] = {&_detail::format_value_thunk<wchar_t, Args>..., nullptr};

	return _detail::printf_impl(writer, format, sizeof...(args), funcs, values);
}

#endif // !defined(_guard_FORMATXX_WIDE_H)
