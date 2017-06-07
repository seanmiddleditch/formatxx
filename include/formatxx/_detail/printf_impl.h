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

#if !defined(_guard_FORMATXX_DETAIL_PRINTF_IMPL_H)
#define _guard_FORMATXX_DETAIL_PRINTF_IMPL_H
#pragma once

namespace formatxx {
namespace _detail {

template <typename CharT>
FORMATXX_PUBLIC basic_format_writer<CharT>& FORMATXX_API printf_impl(basic_format_writer<CharT>& out, basic_string_view<CharT> format, std::size_t count, BasicFormatterThunk<CharT> const* funcs, FormatterParameter const* values)
{
	unsigned next_index = 0;

	CharT const* begin = format.data();
	CharT const* const end = begin + format.size();
	CharT const* iter = begin;

	while (iter < end)
	{
		if (*iter != FormatTraits<CharT>::cPrintfSpec)
		{
			++iter;
		}
		else
		{
			// write out the string so far, since we don't write characters immediately
			if (iter > begin)
				out.write({begin, iter});

			++iter; // swallow the {

			// if we hit the end of the input, we have an incomplete format, and nothing else we can do
			if (iter == end)
			{
				out.write(FormatTraits<CharT>::sErrIncomplete);
				break;
			}

			// if we just have another % then take it as a literal character by starting our next begin here,
			// so it'll get written next time we write out the begin; nothing else to do for formatting here
			if (*iter == FormatTraits<CharT>::cPrintfSpec)
			{
				begin = iter++;
				continue;
			}
		
			// if the index is out of range, we have nothing to format
			if (next_index >= count)
			{
				out.write(FormatTraits<CharT>::sErrOutOfRange);
				continue;
			}

			// parse forward through the specification
			CharT const* const spec_begin = iter;
			basic_format_spec<CharT> spec = parse_format_spec(basic_string_view<CharT>(iter, end));
			CharT const* const spec_end = spec.extra.data();
			if (spec.code == CharT(0))
			{
				// invalid spec
				out.write(FormatTraits<CharT>::sErrBadFormat);
				break;
			}

			// magic!
			funcs[next_index](out, values[next_index], {spec_begin, spec_end});

			// prepare for next round
			begin = iter = spec_end;
			++next_index;
		}
	}

	// write out tail end of format string
	if (iter > begin)
		out.write({begin, iter});

	return out;
}

} // namespace _detail
} // namespace formatxx

#endif // _guard_FORMATXX_DETAIL_PRINTF_IMPL_H
