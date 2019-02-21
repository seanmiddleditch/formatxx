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

#if !defined(FORMATXX_USE_BUILTIN_STRLEN)
#	if (defined(_MSC) && _MSC >= 1700) || (defined(__clang__) || __clang__ >= 4) || (defined __GNUC__ && __GNUC__ >= 5)
#		define FORMATXX_USE_BUILTIN_STRLEN
#	endif
#endif
#if !defined(FORMATXX_USE_BUILTIN_STRLEN)
#	include <string>
#endif

#if !defined(FORMATXX_API)
#	if defined(_WIN32)
#		define FORMATXX_API __stdcall
#	else
#		define FORMATXX_API
#	endif
#endif

#if defined(_WIN32) && !defined(FORMATXX_PUBLIC)
#	if defined(FORMATXX_EXPORT)
#		define FORMATXX_PUBLIC __declspec(dllexport)
#	elif !defined(FORMATXX_STATIC)
#		define FORMATXX_PUBLIC __declspec(dllimport)
#	else
#		define FORMATXX_PUBLIC
#	endif
#elif __GNUC__ >= 4 && !defined(FORMATXX_PUBLIC)
#	if defined(FORMATXX_EXPORT)
#		define FORMATXX_PUBLIC __attribute__((visibility("default")))
#	else
#		define FORMATXX_PUBLIC
#	endif
#endif

namespace formatxx
{
	template <typename CharT> class basic_string_view;
	template <typename CharT> class basic_format_writer;
	template <typename CharT> class basic_format_spec;
	template <typename CharT> class basic_format_args;

	enum class result_code;
	
	using string_view = basic_string_view<char>;
	using format_writer = basic_format_writer<char>;
	using format_spec = basic_format_spec<char>;

	template <typename CharT, typename FormatT, typename... Args> result_code format(basic_format_writer<CharT>& writer, FormatT const& format, Args const&... args);
	template <typename CharT, typename FormatT, typename... Args> result_code printf(basic_format_writer<CharT>& writer, FormatT const& format, Args const&... args);

	template <typename CharT> FORMATXX_PUBLIC basic_format_spec<CharT> FORMATXX_API parse_format_spec(basic_string_view<CharT> spec);

	template <typename CharT> basic_string_view<CharT> make_string_view(basic_string_view<CharT> str) { return str; }
	template <typename CharT> basic_string_view<CharT> make_string_view(CharT const* zstr) { return zstr; }
}

enum class formatxx::result_code
{
	success,
	out_of_range,
	malformed_input,
	out_of_space,
};

/// Describes a format string.
template <typename CharT>
class formatxx::basic_string_view
{
public:
	constexpr basic_string_view() = default;
	constexpr basic_string_view(CharT const* first, CharT const* last) : _begin(first), _size(last - first) {}
	constexpr basic_string_view(CharT const* nstr, std::size_t size) : _begin(nstr), _size(size) {}
#if defined(FORMATXX_USE_BUILTIN_STRLEN)
	basic_string_view(CharT const* zstr) : _begin(zstr), _size(zstr != nullptr ? __builtin_strlen(zstr) : 0) {}
#else
	basic_string_view(CharT const* zstr) : _begin(zstr), _size(zstr != nullptr ? std::char_traits<CharT>::length(zstr) : 0) {}
#endif

	constexpr CharT const* data() const { return _begin; }
	constexpr std::size_t size() const { return _size; }
	constexpr bool empty() const { return _size == 0; }

	constexpr CharT const* begin() const { return _begin; }
	constexpr CharT const* end() const { return _begin + _size; }

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
	virtual void write(basic_string_view<CharT> str) = 0;
};

/// Extra formatting specifications.
template <typename CharT>
class formatxx::basic_format_spec
{
public:
	CharT const* remaining = nullptr;
	unsigned width = 0;
	unsigned precision = 0;
	CharT code = 0;
	bool has_precision = false;
	bool left_justify = false;
	bool prepend_sign = false;
	bool prepend_space = false;
	bool alternate_form = false;
	bool leading_zeroes = false;
};

/// Abstraction for a set of format arguments.
template <typename CharT>
class formatxx::basic_format_args
{
public:
	using thunk_type = result_code(FORMATXX_API *)(basic_format_writer<CharT>&, void const*, basic_string_view<CharT>);
	using size_type = std::size_t;

	basic_format_args() = default;
	explicit basic_format_args(size_type count, thunk_type const* thunks, void const* const* args) : _thunks(thunks), _args(args), _count(count) {}

	result_code format_arg(basic_format_writer<CharT>& output, size_type index, basic_string_view<CharT> spec) const
	{
		return index < _count ? _thunks[index](output, _args[index], spec) : result_code::out_of_range;
	}

private:
	thunk_type const* _thunks = nullptr;
	void const* const* _args = nullptr;
	size_type _count = 0;
};

namespace formatxx
{
	/// Default format helpers.
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, char const* zstr, string_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, char* zstr, string_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, string_view str, string_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, char ch, string_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, bool value, string_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, float value, string_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, double value, string_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, signed char value, string_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, signed int value, string_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, signed long value, string_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, signed short value, string_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, signed long long value, string_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, unsigned char value, string_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, unsigned int value, string_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, unsigned long value, string_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, unsigned short value, string_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, unsigned long long value, string_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, void* value, string_view spec);
	FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, void const* value, string_view spec);

	/// Formatting for enumerations, using their numeric value.
	template <typename CharT, typename EnumT>
	auto FORMATXX_API format_value(basic_format_writer<CharT>& out, EnumT value, string_view spec) -> typename std::enable_if<std::is_enum<EnumT>::value>::type
	{
		format_value(out, typename std::underlying_type<EnumT>::type(value), spec);
	}

	template <typename CharT, typename PointerT>
	auto FORMATXX_API format_value(basic_format_writer<CharT>& out, PointerT value, string_view spec) -> typename std::enable_if<std::is_pointer<PointerT>::value>::type
	{
		format_value(out, static_cast<void const*>(value), spec);
	}

	/// @internal
	namespace _detail
	{
		template <typename CharT, typename T>
		result_code FORMATXX_API format_value_thunk(basic_format_writer<CharT>& out, void const* ptr, basic_string_view<CharT> spec)
		{
			format_value(out, *static_cast<T const*>(ptr), spec);
			return result_code::success;
		}

		template <typename CharT>
		FORMATXX_PUBLIC result_code FORMATXX_API format_impl(basic_format_writer<CharT>& out, basic_string_view<CharT> format, basic_format_args<CharT> args);
		template <typename CharT>
		FORMATXX_PUBLIC result_code FORMATXX_API printf_impl(basic_format_writer<CharT>& out, basic_string_view<CharT> format, basic_format_args<CharT> args);
	}
}

extern template FORMATXX_PUBLIC formatxx::result_code FORMATXX_API formatxx::_detail::format_impl(basic_format_writer<char>& out, basic_string_view<char> format, basic_format_args<char> args);
extern template FORMATXX_PUBLIC formatxx::result_code FORMATXX_API formatxx::_detail::printf_impl(basic_format_writer<char>& out, basic_string_view<char> format, basic_format_args<char> args);	
extern template FORMATXX_PUBLIC formatxx::basic_format_spec<char> FORMATXX_API formatxx::parse_format_spec(basic_string_view<char> spec);

/// Write the string format using the given parameters into a buffer.
/// @param writer The write buffer that will receive the formatted text.
/// @param format The primary text and formatting controls to be written.
/// @param args The arguments used by the formatting string.
template <typename CharT, typename FormatT, typename... Args>
formatxx::result_code formatxx::format(basic_format_writer<CharT>& writer, FormatT const& format, Args const&... args)
{
	void const* const values[] = {std::addressof(args)..., nullptr};
	typename basic_format_args<CharT>::thunk_type const funcs[] = {&_detail::format_value_thunk<CharT, Args>..., nullptr};

	return _detail::format_impl(writer, make_string_view(format), basic_format_args<CharT>(sizeof...(args), funcs, values));
}

/// Write the printf format using the given parameters into a buffer.
/// @param writer The write buffer that will receive the formatted text.
/// @param format The primary text and printf controls to be written.
/// @param args The arguments used by the formatting string.
template <typename CharT, typename FormatT, typename... Args>
formatxx::result_code formatxx::printf(basic_format_writer<CharT>& writer, FormatT const& format, Args const&... args)
{
	void const* const values[] = {std::addressof(args)..., nullptr};
	typename basic_format_args<CharT>::thunk_type const funcs[] = {&_detail::format_value_thunk<CharT, Args>..., nullptr};

	return _detail::printf_impl(writer, make_string_view(format), basic_format_args<CharT>(sizeof...(args), funcs, values));
}

#endif // !defined(_guard_FORMATXX_H)
