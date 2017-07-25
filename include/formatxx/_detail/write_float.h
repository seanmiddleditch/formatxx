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

#if !defined(_guard_FORMATXX_DETAIL_WRITE_FLOAT_H)
#define _guard_FORMATXX_DETAIL_WRITE_FLOAT_H
#pragma once

namespace formatxx {
namespace _detail {

template <typename CharT>
struct sprintf_helper
{
	template <typename... Args>
	int operator()(char* buf, int result, char const* fmt, Args const&... args) const
	{
		return std::snprintf(buf, result, fmt, args...);
	}
};

template <>
struct sprintf_helper<wchar_t>
{
	template <typename... Args>
	int operator()(wchar_t* buf, int result, wchar_t const* fmt, Args const&... args) const
	{
		return std::swprintf(buf, result, fmt, args...);
	}
};

template <typename CharT>
void write_float(basic_format_writer<CharT>& out, double value, basic_string_view<CharT> spec_string)
{
	auto const spec = parse_format_spec(spec_string);

	constexpr std::size_t fmt_buf_size = 10;
	CharT fmt_buf[fmt_buf_size];
	CharT* fmt_ptr = fmt_buf + fmt_buf_size;

	*--fmt_ptr = 0;

	// every sprint call must have a valid code (1)
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
		*--fmt_ptr = spec.code;
		break;
	default:
		*--fmt_ptr = 'f';
		break;
	}

	// this flag is independent of any other (2)
	if (spec.has_precision)
	{
		*--fmt_ptr = '*';
		*--fmt_ptr = '.';
	}

	// this flag is independent of any other (1)
	if (spec.has_width)
	{
		*--fmt_ptr = '*';
	}

	// these flags are mutually exclusive within themselves (1)
	if (spec.prepend_sign)
	{
		*--fmt_ptr = FormatTraits<CharT>::cPlus;
	}
	else if (spec.prepend_space)
	{
		*--fmt_ptr = FormatTraits<CharT>::cSpace;
	}

	// these flags may all be set together (3)
	if (spec.left_justify)
	{
		*--fmt_ptr = '-';
	}
	if (spec.leading_zeroes)
	{
		*--fmt_ptr = '0';
	}
	if (spec.alternate_form)
	{
		*--fmt_ptr = FormatTraits<CharT>::cHash;
	}

	// every format must start with this (1)
	*--fmt_ptr = '%';

	constexpr std::size_t buf_size = 1078;
	CharT buf[buf_size];
	auto const helper = sprintf_helper<CharT>();
	
	int result = 0;
	if (spec.has_width && spec.has_precision)
	{
		result = helper(buf, buf_size, fmt_ptr, spec.width, spec.precision, value);
	}
	else if (spec.has_width)
	{
		result = helper(buf, buf_size, fmt_ptr, spec.width, value);
	}
	else if (spec.has_precision)
	{
		result = helper(buf, buf_size, fmt_ptr, spec.precision, value);
	}
	else
	{
		result = helper(buf, buf_size, fmt_ptr, value);
	}

	if (result > 0)
	{
		out.write({buf, result < buf_size ? std::size_t(result) : buf_size});
	}
}

} // namespace _detail
} // namespace formatxx

#endif // _guard_FORMATXX_DETAIL_WRITE_FLOAT_H
