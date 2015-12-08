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

#if !defined(_guard_STDFMT_H)
#define _guard_STDFMT_H
#pragma once

#include <string>
#include <memory>

namespace formatxx
{
	template <typename TraitsT, typename AllocatorT>
	string_view make_view(std::basic_string<char, TraitsT, AllocatorT> const& string);

	template <typename TraitsT, typename AllocatorT>
	void format_value(format_writer& out, std::basic_string<char, TraitsT, AllocatorT> const& string, format_spec const& spec);

	template <typename = std::string> class string_writer;
	template <std::size_t, typename = std::allocator> class buffered_writer;
}

template <typename TraitsT, typename AllocatorT>
auto formatxx::make_view(std::basic_string<char, TraitsT, AllocatorT> const& string) -> string_view
{
	return string_view(string.c_str(), string.size());
}

template <typename TraitsT, typename AllocatorT>
void formatxx::format_value(format_writer& out, std::basic_string<char, TraitsT, AllocatorT> const& string, format_spec const& spec)
{
	format_value(out, string_view(string.c_str(), string.size()), spec);
}


#endif // defined(_guard_STDFMT_H)
