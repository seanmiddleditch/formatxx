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

#include <formatxx/format.h>
#include <formatxx/wide.h>

#include <formatxx/_detail/format_traits.h>
#include <formatxx/_detail/parse_unsigned.h>
#include <formatxx/_detail/parse_format.h>
#include <formatxx/_detail/write_integer.h>
#include <formatxx/_detail/format_impl.h>
#include <formatxx/_detail/printf_impl.h>

namespace formatxx {

void format_value(wformat_writer& out, wchar_t ch, wstring_view)
{
	out.write(wstring_view(&ch, 1));
}

void format_value(wformat_writer& out, wchar_t* zstr, wstring_view spec)
{
	format_value(out, wstring_view(zstr), spec);
}

void format_value(wformat_writer& out, wchar_t const* zstr, wstring_view spec)
{
	format_value(out, wstring_view(zstr), spec);
}

void format_value(wformat_writer& out, wstring_view str, wstring_view)
{
	out.write(str);
}

namespace _detail {

template basic_format_writer<wchar_t>& format_impl(basic_format_writer<wchar_t>& out, basic_string_view<wchar_t> format, std::size_t count, BasicFormatterThunk<wchar_t> const* funcs, FormatterParameter const* values);

template basic_format_writer<wchar_t>& printf_impl(basic_format_writer<wchar_t>& out, basic_string_view<wchar_t> format, std::size_t count, BasicFormatterThunk<wchar_t> const* funcs, FormatterParameter const* values);

} // namespace _detail

template basic_format_spec<wchar_t> parse_format_spec(basic_string_view<wchar_t>);

} // namespace formatxx