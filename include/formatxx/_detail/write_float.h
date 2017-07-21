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
namespace {

template <typename CharT, typename ValueT, typename FormatFuncT>
void write_float_helper(basic_format_writer<CharT>& out, ValueT value, basic_string_view<CharT> spec_string, FormatFuncT const& formatter)
{
	CharT fmt[3] = {'%', 'f', 0};

	auto const spec = parse_format_spec(spec_string);

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
		fmt[1] = spec.code;
		break;
	default:
		// leave default
		break;
	}

	CharT buf[1078];
	int len = formatter(buf, sizeof(buf) / sizeof(buf[0]), fmt, value);
	if (len > 0)
		out.write({buf, std::size_t(len)});
}

template <typename ValueT>
void write_float(basic_format_writer<char>& out, ValueT value, basic_string_view<char> spec)
{
	write_float_helper(out, value, spec, [](char* buf, int len, char const* fmt, ValueT value){ return std::snprintf(buf, len, fmt, value); });
}

template <typename ValueT>
void write_float(basic_format_writer<wchar_t>& out, ValueT value, basic_string_view<wchar_t> spec)
{
	write_float_helper(out, value, spec, [](wchar_t* buf, int len, wchar_t const* fmt, ValueT value){ return std::swprintf(buf, len, fmt, value); });
}


} // anonymous namespace
} // namespace _detail
} // namespace formatxx

#endif // _guard_FORMATXX_DETAIL_WRITE_FLOAT_H
