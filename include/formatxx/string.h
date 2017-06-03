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

#if !defined(_guard_FORMATXX_STRING_H)
#define _guard_FORMATXX_STRING_H
#pragma once

#include "format.h"

#include <string>

namespace formatxx {

template <typename StringT> class basic_string_writer;

using string_writer = basic_string_writer<std::string>;

template <typename StringT = std::string, typename... Args> StringT sformat(string_view format, Args&&... args);
template <typename StringT = std::string, typename... Args> StringT formatxx::sprintf(string_view format, Args&&... args);

template <typename CharT, typename CharTraitsT, typename AllocatorT, typename... Args>
format_writer& format(format_writer& writer, std::basic_string<CharT, CharTraitsT, AllocatorT> format, Args&&... args);
template <typename CharT, typename CharTraitsT, typename AllocatorT, typename... Args>
format_writer& printf(format_writer& writer, std::basic_string<CharT, CharTraitsT, AllocatorT> format, Args&&... args);

template <typename TraitsT, typename AllocatorT>
void format_value(format_writer& out, std::basic_string<char, TraitsT, AllocatorT> const& string, string_view spec)
{
    format_value(out, string_view(string), spec);
}

} // namespace formatxx

/// A writer that generates a buffer (intended for std::string).
template <typename StringT>
class formatxx::basic_string_writer : public basic_format_writer<typename StringT::value_type>
{   
public:
    basic_string_writer() = default;
    basic_string_writer(StringT init) : _string(std::move(init)) {}

	void write(basic_string_view<typename StringT::value_type> str) override { _string.append(str.data(), str.size()); }
	basic_string_view<typename StringT::value_type> view() const override { return {_string.c_str(), _string.size()}; }

	StringT const& str() const& { return _string; }
	StringT& str() & { return _string; }
    StringT&& str() && { return std::move(_string); }

	void clear() { _string.clear(); }
	std::size_t size() const { return _string.size(); }
	typename StringT::value_type const* c_str() const { return _string.c_str(); }

private:
	StringT _string;
};

/// Write the string format using the given parameters into a buffer.
/// @param writer The write buffer that will receive the formatted text.
/// @param format The primary text and formatting controls to be written.
/// @param args The arguments used by the formatting string.
template <typename CharT, typename CharTraitsT, typename AllocatorT, typename... Args>
formatxx::format_writer& formatxx::format(format_writer& writer, std::basic_string<CharT, CharTraitsT, AllocatorT> format, Args&&... args)
{
    formatxx::format(writer, basic_string_view<CharT>(format.data(), format.size()), std::forward<Args>(args)...);
}

/// Write the printf format using the given parameters into a buffer.
/// @param writer The write buffer that will receive the formatted text.
/// @param format The primary text and printf controls to be written.
/// @param args The arguments used by the formatting string.
template <typename CharT, typename CharTraitsT, typename AllocatorT, typename... Args>
formatxx::format_writer& formatxx::printf(format_writer& writer, std::basic_string<CharT, CharTraitsT, AllocatorT> format, Args&&... args)
{
	constexpr auto count = sizeof...(args);
	void const* values[count] = {std::addressof(static_cast<std::decay_t<decltype(args)> const&>(args))...};
	constexpr _detail::FormatterThunk funcs[count] = {&_detail::wrap<std::decay_t<Args>>::fwd...};

	_detail::printf_impl(writer, basic_string_view<CharT>(format.data(), format.size()), count, funcs, values);

	return writer;
}

/// Write the string format using the given parameters and return a string with the result.
/// @param format The primary text and formatting controls to be written.
/// @param args The arguments used by the formatting string.
/// @returns a formatted string.
template <typename StringT, typename... Args>
StringT formatxx::sformat(string_view format, Args&&... args)
{
	basic_string_writer<StringT> tmp;
	formatxx::format(tmp, format, std::forward<Args>(args)...);
	return static_cast<StringT&&>(tmp.str());
}

/// Write the printf format using the given parameters and return the result.
/// @param writer The write buffer that will receive the formatted text.
/// @param format The primary text and printf controls to be written.
/// @param args The arguments used by the formatting string.
template <typename StringT, typename... Args>
StringT formatxx::sprintf(string_view format, Args&&... args)
{
	basic_string_writer<StringT> tmp;
	formatxx::printf(tmp, format, std::forward<Args>(args)...);
	return static_cast<StringT&&>(tmp.str());
}

#endif // !defined(_guard_FORMATXX_STRING_H)
