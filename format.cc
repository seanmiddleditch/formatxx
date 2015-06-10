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

#include "format.h"

#include <cstdio>

namespace
{
	const char sErrBadFormat[] =  "#BADF";
	const char sErrIncomplete[] = "#INCL";
	const char sErrOutOfRange[] = "#RNGE";

	template <size_t N>
	void write_error(fmt2::IWriter& writer, const char(&string)[N])
	{
		writer.Write(string, N - 1);
	}

	template <typename T>
	void write_signed(fmt2::IWriter& writer, T const& value, fmt2::FormatSpec const&)
	{
		char buf[48];
		int len = std::snprintf(buf, sizeof(buf), "%li", static_cast<signed long>(value));
		writer.Write(buf, len);
	}

	template <typename T>
	void write_unsigned(fmt2::IWriter& writer, T const& value, fmt2::FormatSpec const&)
	{
		char buf[48];
		int len = std::snprintf(buf, sizeof(buf), "%lu", static_cast<unsigned long>(value));
		writer.Write(buf, len);
	}

	char const* parse_unsigned(char const* start, char const* end, unsigned* result)
	{
		unsigned tmp = 0;
		while (start != end && *start >= '0' && *start <= '9')
		{
			tmp *= 10;
			tmp += *start - '0';
			++start;
		}
		*result = tmp;
		return start;
	}
}

namespace fmt2
{
	void format(IWriter& writer, char ch, FormatSpec const&)
	{
		writer.Write(&ch, 1);
	}

	void format(IWriter& writer, bool value, FormatSpec const&)
	{
		value ? write_error(writer, "true") : write_error(writer, "false");
	}

	void format(IWriter& writer, char const* zstr, FormatSpec const&)
	{
		auto const len = std::strlen(zstr);
		writer.Write(zstr, len);
	}

	void format(IWriter& writer, StringView str, FormatSpec const&)
	{
		writer.Write(str.begin, str.end - str.begin);
	}

	void format(IWriter& writer, signed int value, FormatSpec const& spec) { write_signed(writer, value, spec); }
	void format(IWriter& writer, signed long value, FormatSpec const& spec) { write_signed(writer, value, spec); }
	void format(IWriter& writer, signed short value, FormatSpec const& spec) { write_signed(writer, value, spec); }

	void format(IWriter& writer, signed long long value, FormatSpec const& spec)
	{
		char buf[48];
		int len = std::snprintf(buf, sizeof(buf), "%lli", value);
		writer.Write(buf, len);
	}

	void format(IWriter& writer, unsigned int value, FormatSpec const& spec) { write_unsigned(writer, value, spec); }
	void format(IWriter& writer, unsigned long value, FormatSpec const& spec) { write_unsigned(writer, value, spec); }
	void format(IWriter& writer, unsigned short value, FormatSpec const& spec) { write_unsigned(writer, value, spec); }
	
	void format(IWriter& writer, unsigned long long value, FormatSpec const& spec)
	{
		char buf[48];
		int len = std::snprintf(buf, sizeof(buf), "%llu", value);
		writer.Write(buf, len);
	}

	void format(IWriter& writer, float value, FormatSpec const&)
	{
		char buf[512]; // not actually enough for every float, but...
		int len = std::snprintf(buf, sizeof(buf), "%f", value);
		writer.Write(buf, len);
	}

	void format(IWriter& writer, void const* ptr, FormatSpec const&)
	{
		char buf[48];
		int len = std::snprintf(buf, sizeof(buf), "%p", ptr);
		writer.Write(buf, len);
	}
}

void fmt2::_detail::format_impl(IWriter& writer, StringView format, std::size_t count, FormatFunc const* funcs, void const** values)
{
	unsigned next_index = 0;

	char const* span = format.begin;
	char const* cur = format.begin;
	while (cur < format.end)
	{
		if (*cur == '{')
		{
			// write out the string so far, since we don't write characters immediately
			if (cur > span)
			{
				writer.Write(span, cur - span);
			}

			++cur; // swallow the {

			// if we hit the end of the input, we have an incomplete format, and nothing else we can do
			if (cur == format.end)
			{
				write_error(writer, sErrIncomplete);
				break;
			}

			// if we just have another { then take it as a literal character by starting our next span here,
			// so it'll get written next time we write out the span; nothing else to do for formatting here
			if (*cur == '{')
			{
				span = cur++;
				continue;
			}

			// determine which argument we're going to format
			unsigned index = 0;
			char const* end = parse_unsigned(cur, format.end, &index);

			// if we read nothing, we have a "next index" situation (or an error)
			if (end == cur)
				index = next_index;

			// if we hit the end of the string, we have an incomplete format
			if (cur == format.end)
			{
				write_error(writer, sErrIncomplete);
				break;
			}

			// if a | follows the number, we have some formatting controls
			if (*end == '|')
			{
				// #FIXME: parse and save these
				while (end < format.end && *end != '}')
					++end;

				if (end == format.end)
				{
					write_error(writer, sErrIncomplete);
					break;
				}
			}
			else if (*end != '}')
			{
				// we have something besides a number, no bueno
				write_error(writer, sErrIncomplete);
				span = cur = end; // make sure we're set up for the next span, which starts at this unknown character
				continue;
			}

			// the current text span begins with the next character following the format directive's end
			span = cur = end + 1;

			// if the index is out of range, we have nothing to format
			if (index >= count)
			{
				write_error(writer, sErrOutOfRange);
				continue;
			}

			// magic!
			FormatSpec spec;
			FormatFunc func = funcs[index];
			void const* value = values[index];
			func(writer, value, spec);

			// if we continue to receive {} then the next index will be the next one after the last one used
			next_index = index + 1;
		}
		else
		{
			++cur;
		}
	}

	// write out tail end of format string
	if (cur > span)
		writer.Write(span, cur - span);
}
