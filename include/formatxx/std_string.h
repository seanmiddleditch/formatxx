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

#include <formatxx/format.h>
#include <formatxx/writers.h>
#include <string>

namespace formatxx
{
	template <typename StringT = std::string, typename FormatT, typename... Args> StringT format_string(FormatT const& format, Args const& ... args);
	template <typename StringT = std::string, typename FormatT, typename... Args> StringT printf_string(FormatT const& format, Args const& ... args);

	template <typename CharT, typename TraitsT, typename AllocatorT>
	void format_value(format_writer& out, std::basic_string<CharT, TraitsT, AllocatorT> const& string, string_view spec)
	{
		format_value(out, basic_string_view<CharT>(string.c_str(), string.size()), spec);
	}

	template <typename CharT, typename TraitsT, typename AllocatorT>
	void format_value(basic_format_writer<wchar_t>& out, std::basic_string<CharT, TraitsT, AllocatorT> const& string, basic_string_view<wchar_t> spec)
	{
		format_value(out, basic_string_view<CharT>(string.c_str(), string.size()), spec);
	}

} // namespace formatxx

/// Write the string format using the given parameters and return a string with the result.
/// @param format The primary text and formatting controls to be written.
/// @param args The arguments used by the formatting string.
/// @returns a formatted string.
template <typename StringT, typename FormatT, typename... Args>
StringT formatxx::format_string(FormatT const& format, Args const& ... args)
{
    StringT result;
    append_writer writer(result);
	formatxx::format_to(writer, basic_string_view<typename StringT::value_type>(format), args...);
	return result;
}

/// Write the printf format using the given parameters and return a string with the result.
/// @param format The primary text and printf controls to be written.
/// @param args The arguments used by the formatting string.
template <typename StringT, typename FormatT, typename... Args>
StringT formatxx::printf_string(FormatT const& format, Args const& ... args)
{
    StringT result;
    append_writer writer(result);
    formatxx::printf_to(writer, basic_string_view<typename StringT::value_type>(format), args...);
    return result;
}

#endif // !defined(_guard_FORMATXX_STRING_H)
