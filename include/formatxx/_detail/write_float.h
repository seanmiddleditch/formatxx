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

#include <cstdarg>

namespace formatxx {
namespace _detail {
namespace {

template <typename CharT, typename FormatFuncT>
void write_float_helper(basic_format_writer<CharT>& out, double value, basic_string_view<CharT> spec_string, FormatFuncT const& formatter)
{
	auto const spec = parse_format_spec(spec_string);

	CharT fmt_buf[9];
	CharT* fmt_ptr = fmt_buf;

	*fmt_ptr++ = '%';

	if (spec.sign)
	{
		*fmt_ptr++ = spec.sign;
	}
	if (spec.pad == '0')
	{
		*fmt_ptr++ = spec.pad;
	}
	if (spec.type_prefix)
	{
		*fmt_ptr++ = FormatTraits<CharT>::cHash;
	}
	if (spec.width)
	{
		*fmt_ptr++ = '*';
	}
	if (spec.precision)
	{
		*fmt_ptr++ = '.';
		*fmt_ptr++ = '*';
	}

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
		*fmt_ptr++ = spec.code;
		break;
	default:
		*fmt_ptr++ = 'f';
		break;
	}

	*fmt_ptr = 0; // NUL terminate format buffer

	CharT buf[1078];
	int len = 0;
	
	if (spec.width && spec.precision)
	{
		len = formatter(buf, sizeof(buf) / sizeof(buf[0]), fmt_buf, spec.width, spec.precision, value);
	}
	else if (spec.width)
	{
		len = formatter(buf, sizeof(buf) / sizeof(buf[0]), fmt_buf, spec.width, value);
	}
	else if (spec.precision)
	{
		len = formatter(buf, sizeof(buf) / sizeof(buf[0]), fmt_buf, spec.precision, value);
	}
	else
	{
		len = formatter(buf, sizeof(buf) / sizeof(buf[0]), fmt_buf, value);
	}

	if (len > 0)
	{
		out.write({buf, std::size_t(len)});
	}
}

template <typename ValueT>
void write_float(basic_format_writer<char>& out, ValueT value, basic_string_view<char> spec)
{
	write_float_helper(out, value, spec, [](char* buf, int len, char const* fmt, ...)
	{
		va_list va;
		va_start(va, fmt);
		int const rs = std::vsnprintf(buf, len, fmt, va);
		va_end(va);
		return rs;
	});
}

template <typename ValueT>
void write_float(basic_format_writer<wchar_t>& out, ValueT value, basic_string_view<wchar_t> spec)
{
	write_float_helper(out, value, spec, [](wchar_t* buf, int len, wchar_t const* fmt, ...)
	{
		va_list va;
		va_start(va, fmt);
		int const rs = std::vswprintf(buf, len, fmt, va);
		va_end(va);
		return rs;
	});
}


} // anonymous namespace
} // namespace _detail
} // namespace formatxx

#endif // _guard_FORMATXX_DETAIL_WRITE_FLOAT_H
