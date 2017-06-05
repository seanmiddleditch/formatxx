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

#if !defined(_guard_FORMATXX_H)
#define _guard_FORMATXX_H
#pragma once

#include <type_traits>
#include <cstring>
#include <string>

namespace formatxx
{
	template <typename CharT> class basic_string_view;
	template <typename CharT> class basic_format_writer;
	template <typename CharT, std::size_t> class basic_fixed_writer;

	struct format_spec;
	
	using string_view = basic_string_view<char>;
	using format_writer = basic_format_writer<char>;
	template <std::size_t Size = 512> using fixed_writer = basic_fixed_writer<char, Size>;

	template <typename... Args> format_writer& format(format_writer& writer, string_view format, Args const&... args);
	template <typename... Args> format_writer& printf(format_writer& writer, string_view format, Args const&... args);

	format_spec parse_format_spec(string_view spec);
}

/// Describes a format string.
template <typename CharT>
class formatxx::basic_string_view
{
public:
	constexpr basic_string_view() = default;
	constexpr basic_string_view(CharT const* first, CharT const* last) : _begin(first), _size(last - first) {}
	constexpr basic_string_view(CharT const* nstr, std::size_t size) : _begin(nstr), _size(size) {}
	basic_string_view(CharT const* zstr) : _begin(zstr), _size(std::char_traits<CharT>::length(zstr)) {}

	constexpr CharT const* data() const { return _begin; }
	constexpr std::size_t size() const { return _size; }
	constexpr bool empty() const { return _size == 0; }

private:
	CharT const* _begin = nullptr;
	std::size_t _size = 0;
};

/// Interface for any buffer that the format library can write into.
template <typename CharT>
class formatxx::basic_format_writer
{
public:
	virtual ~basic_format_writer() = default;

	/// Write a string slice.
	/// @param str The string to write.
	virtual void write(string_view str) = 0;

	/// Extract the current value of the writer.
	virtual string_view view() const = 0;
};

/// A writer with a fixed buffer that will never allocate.
template <typename CharT, std::size_t SizeN>
class formatxx::basic_fixed_writer : public basic_format_writer<CharT>
{
public:
	void write(basic_string_view<CharT> str) override;
	basic_string_view<CharT> view() const override { return basic_string_view<CharT>(_buffer, _last); }

	void clear() { _last = _buffer; }
	std::size_t size() const { return _last - _buffer; }
	CharT const* c_str() const { return _buffer; }

private:
	CharT* _last = _buffer;
	CharT _buffer[SizeN] = {CharT(0),};
};

/// Extra formatting specifications.
struct formatxx::format_spec
{
	char code = '\0';
	bool type_prefix = false; // print leading 0x or appropriate type
	enum
	{
		sign_default, // print sign for negative numbers
		sign_always, // print sign for all numbers
		sign_space, // print for for negative numbers or a space for non-negative numbers
	} sign = sign_default;
	formatxx::string_view extra;
};

namespace formatxx
{
	/// Default format helpers.
	void format_value(format_writer& out, char* zstr, string_view spec);
	void format_value(format_writer& out, char const* zstr, string_view spec);
	void format_value(format_writer& out, string_view str, string_view spec);
	void format_value(format_writer& out, char ch, string_view spec);
	void format_value(format_writer& out, bool value, string_view spec);
	void format_value(format_writer& out, float value, string_view spec);
	void format_value(format_writer& out, double value, string_view spec);
	void format_value(format_writer& out, signed char value, string_view spec);
	void format_value(format_writer& out, signed int value, string_view spec);
	void format_value(format_writer& out, signed long value, string_view spec);
	void format_value(format_writer& out, signed short value, string_view spec);
	void format_value(format_writer& out, signed long long value, string_view spec);
	void format_value(format_writer& out, unsigned char value, string_view spec);
	void format_value(format_writer& out, unsigned int value, string_view spec);
	void format_value(format_writer& out, unsigned long value, string_view spec);
	void format_value(format_writer& out, unsigned short value, string_view spec);
	void format_value(format_writer& out, unsigned long long value, string_view spec);
	void format_value(format_writer& out, void* value, string_view spec);
	void format_value(format_writer& out, void const* value, string_view spec);

	/// Formatting for enumerations, using their numeric value.
	template <typename EnumT>
	auto format_value(format_writer& out, EnumT value, string_view spec) -> std::enable_if_t<std::is_enum<EnumT>::value>
	{
		format_value(out, std::underlying_type_t<EnumT>(value), spec);
	}

	template <typename PointerT>
	auto format_value(format_writer& out, PointerT value, string_view spec) -> std::enable_if_t<std::is_pointer<PointerT>::value>
	{
		format_value(out, static_cast<void const*>(value), spec);
	}

	/// Cause a friendlier error message on unknown type.
	template <typename T>
	auto format_value(format_writer& writer, T const& value, string_view spec) -> std::enable_if_t<!std::is_enum<T>::value && !std::is_pointer<T>::value> = delete;

	/// @internal
	namespace _detail
	{
		using FormatterThunk = void(*)(format_writer&, void const*, string_view);
		using Formatter = void(*)(format_writer&, string_view, std::size_t, FormatterThunk const*, void const**);

		template <typename T> void format_value_thunk(format_writer& out, void const* ptr, string_view spec) { format_value(out, *static_cast<T const*>(ptr), spec); }

		void format_impl(format_writer& out, string_view format, std::size_t count, FormatterThunk const* funcs, void const** values);
		void printf_impl(format_writer& out, string_view format, std::size_t count, FormatterThunk const* funcs, void const** values);

		template <Formatter Func, typename... Args>
		format_writer& invoke_format(format_writer& writer, string_view format, Args const&... args)
		{
			void const* values[] = {std::addressof(static_cast<std::decay_t<decltype(args)> const&>(args))..., nullptr};
			constexpr FormatterThunk funcs[] = {&format_value_thunk<std::decay_t<decltype(args)>>..., nullptr};

			Func(writer, format, sizeof...(args), funcs, values);

			return writer;
		}
	}
}

/// Write the string format using the given parameters into a buffer.
/// @param writer The write buffer that will receive the formatted text.
/// @param format The primary text and formatting controls to be written.
/// @param args The arguments used by the formatting string.
template <typename... Args>
formatxx::format_writer& formatxx::format(format_writer& writer, string_view format, Args const&... args)
{
	return _detail::invoke_format<_detail::format_impl>(writer, format, args...);
}

/// Write the printf format using the given parameters into a buffer.
/// @param writer The write buffer that will receive the formatted text.
/// @param format The primary text and printf controls to be written.
/// @param args The arguments used by the formatting string.
template <typename... Args>
formatxx::format_writer& formatxx::printf(format_writer& writer, string_view format, Args const&... args)
{
	return _detail::invoke_format<_detail::printf_impl>(writer, format, args...);
}

template <typename CharT, std::size_t SizeN>
void formatxx::basic_fixed_writer<CharT, SizeN>::write(basic_string_view<CharT> str)
{
	std::size_t const remaining = SizeN - size() - 1;
	std::size_t const length = remaining < str.size() ? remaining : str.size();
	std::memcpy(_last, str.data(), length * sizeof(CharT));
	_last += length;
	*_last = CharT(0);
}

#endif // !defined(_guard_FORMATXX_H)
