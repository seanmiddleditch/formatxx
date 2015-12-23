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

#include "formatxx/format.h"

#include <cstdio>
#include <limits>

namespace
{
	constexpr char sErrBadFormat[]  = "#BADF";
	constexpr char sErrIncomplete[] = "#INCL";
	constexpr char sErrOutOfRange[] = "#RNGE";

	// during integer formatting, we'll work on every two decimal digits (groups of 100). notes taken from cppformat,
	// which took the notes from Alexandrescu from "Three Optimization Tips for C++"
	constexpr char sDecimalTable[] =
		"00010203040506070809"
		"10111213141516171819"
		"20212223242526272829"
		"30313233343536373839"
		"40414243444546474849"
		"50515253545556575859"
		"60616263646566676869"
		"70717273747576777879"
		"80818283848586878889"
		"90919293949596979899";

	// base-10 digits, NUL, sign, signed extension
	constexpr std::size_t kIntBufferSize = std::numeric_limits<unsigned long long>::digits10 + 3;

	char* gen_unsigned_decimal(char buffer[kIntBufferSize], unsigned long long value)
	{
		// we're going to write backwards into the buffer, starting from the end character
		char* end = buffer + kIntBufferSize;

		// ensure we get the NUL character
		*--end = '\0';

		// work on every two decimal digits (groups of 100). notes taken from cppformat,
		// which took the notes from Alexandrescu from "Three Optimization Tips for C++"
		while (value >= 100)
		{
			// I feel like we could do the % and / better... somehow
			// we multiply the index by two to find the pair of digits to index
			unsigned const digit = (value % 100) << 1;
			value /= 100;

			// write out both digits of the given index, starting with the second, since we're writing backwards
			*--end = sDecimalTable[digit + 1];
			*--end = sDecimalTable[digit];
		}

		if (value >= 10)
		{
			// we have two difits left; this is identical to the above loop, but without the division
			unsigned const digit = static_cast<unsigned>(value << 1);
			*--end = sDecimalTable[digit + 1];
			*--end = sDecimalTable[digit];
		}
		else
		{
			// we have but a single digit left, so this is easy
			*--end = static_cast<char>('0' + value);
		}

		// we return the location in the buffer that our formatted string starts at
		return end;
	}

	char* gen_signed_decimal(char buffer[kIntBufferSize], signed long long value)
	{
		// if the value is non-negative, the unsigned version does everything we need
		if (value >= 0)
			return gen_unsigned_decimal(buffer, static_cast<unsigned long long>(value));

		// subtract form 0 _after_ converting to deal with 2's complement format
		unsigned long long abs_value = 0 - static_cast<unsigned long long>(value);
		char* end = gen_unsigned_decimal(buffer, abs_value);
		*--end = '-';
		return end;
	}

	template <size_t N>
	void write_error(formatxx::format_writer& out, const char(&string)[N])
	{
		out.write(string, N - 1);
	}

	template <typename T>
	void write_unsigned(formatxx::format_writer& out, T value, formatxx::format_spec const&)
	{
		char buffer[kIntBufferSize];
		out.write(gen_unsigned_decimal(buffer, value));
	}

	template <typename T>
	void write_signed(formatxx::format_writer& out, T value, formatxx::format_spec const& spec)
	{
		char buffer[kIntBufferSize];
		out.write(gen_signed_decimal(buffer, value));
	}

	char const* parse_unsigned(char const* start, char const* end, unsigned* result)
	{
		*result = 0;
		while (start != end && *start >= '0' && *start <= '9')
		{
			*result *= 10;
			*result += *start - '0';
			++start;
		}
		return start;
	}
}

namespace formatxx
{
	void format_value(format_writer& out, char ch, format_spec const&)
	{
		out.write(&ch, 1);
	}

	void format_value(format_writer& out, bool value, format_spec const&)
	{
		value ? write_error(out, "true") : write_error(out, "false");
	}

	void format_value(format_writer& out, char* ptr, format_spec const& spec)
	{
		format_value(out, static_cast<char const*>(ptr), spec);
	}

	void format_value(format_writer& out, char const* zstr, format_spec const&)
	{
		auto const len = std::strlen(zstr);
		out.write(zstr, len);
	}

	void format_value(format_writer& out, string_view str, format_spec const&)
	{
		out.write(str.begin, str.end - str.begin);
	}

	void format_value(format_writer& out, signed int value, format_spec const& spec) { write_signed(out, value, spec); }
	void format_value(format_writer& out, signed long value, format_spec const& spec) { write_signed(out, value, spec); }
	void format_value(format_writer& out, signed short value, format_spec const& spec) { write_signed(out, value, spec); }
	void format_value(format_writer& out, signed long long value, format_spec const& spec) { write_signed(out, value, spec); }

	void format_value(format_writer& out, unsigned int value, format_spec const& spec) { write_unsigned(out, value, spec); }
	void format_value(format_writer& out, unsigned long value, format_spec const& spec) { write_unsigned(out, value, spec); }
	void format_value(format_writer& out, unsigned short value, format_spec const& spec) { write_unsigned(out, value, spec); }
	void format_value(format_writer& out, unsigned long long value, format_spec const& spec) { write_unsigned(out, value, spec); }

	void format_value(format_writer& out, float value, format_spec const&)
	{
		char buf[512]; // not actually enough for every float, but...
		int len = std::snprintf(buf, sizeof(buf), "%f", value);
		out.write(buf, len);
	}

	void format_value(format_writer& out, double value, format_spec const&)
	{
		char buf[1048]; // not actually enough for every float, but...
		int len = std::snprintf(buf, sizeof(buf), "%g", value);
		out.write(buf, len);
	}

	void format_value(format_writer& out, void* ptr, format_spec const& spec)
	{
		format_value(out, static_cast<void const*>(ptr), spec);
	}

	void format_value(format_writer& out, void const* ptr, format_spec const&)
	{
		char buf[48];
		int len = std::snprintf(buf, sizeof(buf), "%p", ptr);
		out.write(buf, len);
	}
}

void formatxx::_detail::format_impl(format_writer& out, string_view format, std::size_t count, FormatFunc const* funcs, void const** values)
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
				out.write(span, cur - span);

			++cur; // swallow the {

			// if we hit the end of the input, we have an incomplete format, and nothing else we can do
			if (cur == format.end)
			{
				write_error(out, sErrIncomplete);
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
			char const* const start = cur;
			char const* cur = parse_unsigned(start, format.end, &index);

			// if we read nothing, we have a "next index" situation (or an error)
			if (cur == start)
				index = next_index;

			// if we hit the end of the string, we have an incomplete format
			if (cur == format.end)
			{
				write_error(out, sErrIncomplete);
				break;
			}

			// if a | follows the number, we have some formatting controls
			if (*cur == '|')
			{
				// #FIXME: parse and save these
				while (cur < format.end && *cur != '}')
					++cur;

				if (cur == format.end)
				{
					write_error(out, sErrIncomplete);
					break;
				}
			}
			else if (*cur != '}')
			{
				// we have something besides a number, no bueno
				write_error(out, sErrIncomplete);
				span = cur; // make sure we're set up for the next span, which starts at this unknown character
				continue;
			}

			// the current text span begins with the next character following the format directive's end
			span = cur = cur + 1;

			// if the index is out of range, we have nothing to format
			if (index >= count)
			{
				write_error(out, sErrOutOfRange);
				continue;
			}

			// magic!
			format_spec spec;
			funcs[index](out, values[index], spec);

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
		out.write(span, cur - span);
}
