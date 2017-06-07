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

namespace formatxx {
namespace _detail {

template <typename CharT>
basic_format_writer<CharT>& format_impl(basic_format_writer<CharT>& out, basic_string_view<CharT> format, std::size_t count, BasicFormatterThunk<CharT> const* funcs, FormatterParameter const* values)
{
	unsigned next_index = 0;

	CharT const* begin = format.data();
	CharT const* const end = begin + format.size();
	CharT const* iter = begin;

	while (iter < end)
	{
		if (*iter != FormatTraits<CharT>::cFormatBegin)
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

			// if we just have another { then take it as a literal character by starting our next begin here,
			// so it'll get written next time we write out the begin; nothing else to do for formatting here
			if (*iter == FormatTraits<CharT>::cFormatBegin)
			{
				begin = iter++;
				continue;
			}

			// determine which argument we're going to format
			unsigned index = 0;
			CharT const* const start = iter;
			iter = parse_unsigned(start, end, index);

			// if we read nothing, we have a "next index" situation (or an error)
			if (iter == start)
				index = next_index;

			// if we hit the end of the string, we have an incomplete format
			if (iter == end)
			{
				out.write(FormatTraits<CharT>::sErrIncomplete);
				break;
			}

			basic_string_view<CharT> spec;

			// if a : follows the number, we have some formatting controls
			if (*iter == FormatTraits<CharT>::cFormatSep)
			{
				++iter; // eat separator
				CharT const* const spec_begin = iter;

				while (iter < end && *iter != FormatTraits<CharT>::cFormatEnd)
				{
					++iter;
				}

				if (iter == end)
				{
					// invalid spec
					out.write(FormatTraits<CharT>::sErrBadFormat);
					break;
				}

				spec = basic_string_view<CharT>(spec_begin, iter);
			}

			// after the index/spec, we expect an end to the format marker
			if (*iter != FormatTraits<CharT>::cFormatEnd)
			{
				// we have something besides a number, no bueno
				out.write(FormatTraits<CharT>::sErrIncomplete);
				begin = iter; // make sure we're set up for the next begin, which starts at this unknown character
				continue;
			}

			// the iterrent text begin begins with the next character following the format directive's end
			begin = iter = iter + 1;

			// if the index is out of range, we have nothing to format
			if (index >= count)
			{
				out.write(FormatTraits<CharT>::sErrOutOfRange);
				continue;
			}

			// magic!
			funcs[index](out, values[index], spec);

			// if we continue to receive {} then the next index will be the next one after the last one used
			next_index = index + 1;
		}
	}

	// write out tail end of format string
	if (iter > begin)
		out.write({begin, iter});

	return out;
}

} // namespace _detail
} // namespace formatxx