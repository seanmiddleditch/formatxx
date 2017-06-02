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

#include "write_integer.inl"

#include <cstdio>
#include <limits>
#include <climits>

namespace formatxx {

namespace {

// string_view over string literals is safe on all platforms for which I'm aware
constexpr string_view sErrBadFormat{"#BADF", 5};
constexpr string_view sErrIncomplete{"#INCL", 5};
constexpr string_view sErrOutOfRange{"#RNGE", 5};

char const* parse_unsigned(char const* start, char const* end, unsigned& result);
char const* parse_spec(char const* start, char const* end, format_spec& result);

} // anonymous namespace

format_writer& format(format_writer& writer, string_view format)
{
	_detail::format_impl(writer, format, 0, nullptr, nullptr);
	return writer;
}

void format_value(format_writer& out, char ch, format_spec const&)
{
	out.write(string_view(&ch, 1));
}

void format_value(format_writer& out, bool value, format_spec const&)
{
	out.write(value ? "true" : "false");
}

void format_value(format_writer& out, char* ptr, format_spec const& spec)
{
	if (ptr != nullptr)
		format_value(out, static_cast<char const*>(ptr), spec);
	else
		format_value(out, static_cast<void const*>(nullptr), spec);
}

void format_value(format_writer& out, char const* zstr, format_spec const& spec)
{
	if (zstr != nullptr)
		out.write(string_view(zstr));
	else
		format_value(out, static_cast<void const*>(nullptr), spec);
}

void format_value(format_writer& out, string_view str, format_spec const&)
{
	out.write(str);
}

void format_value(format_writer& out, signed int value, format_spec const& spec) { write_integer(out, value, spec); }
void format_value(format_writer& out, signed char value, format_spec const& spec) { write_integer(out, value, spec); }
void format_value(format_writer& out, signed long value, format_spec const& spec) { write_integer(out, value, spec); }
void format_value(format_writer& out, signed short value, format_spec const& spec) { write_integer(out, value, spec); }
void format_value(format_writer& out, signed long long value, format_spec const& spec) { write_integer(out, value, spec); }

void format_value(format_writer& out, unsigned int value, format_spec const& spec) { write_integer(out, value, spec); }
void format_value(format_writer& out, unsigned char value, format_spec const& spec) { write_integer(out, value, spec); }
void format_value(format_writer& out, unsigned long value, format_spec const& spec) { write_integer(out, value, spec); }
void format_value(format_writer& out, unsigned short value, format_spec const& spec) { write_integer(out, value, spec); }
void format_value(format_writer& out, unsigned long long value, format_spec const& spec) { write_integer(out, value, spec); }

void format_value(format_writer& out, float value, format_spec const& spec)
{
	format_value(out, static_cast<double>(value), spec);
}

void format_value(format_writer& out, double value, format_spec const& spec)
{
	char fmt[3] = "%f";

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

	char buf[1048]; // not actually enough for every float, but...
	int len = std::snprintf(buf, sizeof(buf), fmt, value);
	if (len > 0)
		out.write(string_view(buf, len));
}

void format_value(format_writer& out, void* ptr, format_spec const& spec)
{
	write_integer(out, reinterpret_cast<std::uintptr_t>(ptr), spec);
}

void format_value(format_writer& out, void const* ptr, format_spec const& spec)
{
	write_integer(out, reinterpret_cast<std::uintptr_t>(ptr), spec);
}

namespace _detail {

void format_impl(format_writer& out, string_view format, std::size_t count, FormatFunc const* funcs, void const** values)
{
	unsigned next_index = 0;

	char const* span = format.data();
	char const* cur = span;
	char const* const end = span + format.size();
	while (cur < end)
	{
		if (*cur == '{')
		{
			// write out the string so far, since we don't write characters immediately
			if (cur > span)
				out.write(string_view(span, cur - span));

			++cur; // swallow the {

			// if we hit the end of the input, we have an incomplete format, and nothing else we can do
			if (cur == end)
			{
				out.write(sErrIncomplete);
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
			char const* cur = parse_unsigned(start, end, index);

			// if we read nothing, we have a "next index" situation (or an error)
			if (cur == start)
				index = next_index;

			// if we hit the end of the string, we have an incomplete format
			if (cur == end)
			{
				out.write(sErrIncomplete);
				break;
			}

			format_spec spec;

			// if a : follows the number, we have some formatting controls
			if (*cur == ':')
			{
				cur = parse_spec(++cur, end, spec);

				if (cur == end)
				{
					// invalid spec
					out.write(sErrBadFormat);
					break;
				}
			}

			// after the index/spec, we expect an end to the format marker
			if (*cur != '}')
			{
				// we have something besides a number, no bueno
				out.write(sErrIncomplete);
				span = cur; // make sure we're set up for the next span, which starts at this unknown character
				continue;
			}

			// the current text span begins with the next character following the format directive's end
			span = cur = cur + 1;

			// if the index is out of range, we have nothing to format
			if (index >= count)
			{
				out.write(sErrOutOfRange);
				continue;
			}

			// magic!
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
		out.write(string_view(span, cur - span));
}

} // namespace _detail

namespace {

char const* parse_unsigned(char const* start, char const* end, unsigned& result)
{
	result = 0;
	while (start != end && *start >= '0' && *start <= '9')
	{
		result *= 10;
		result += *start - '0';
		++start;
	}
	return start;
}

char const* parse_spec(char const* start, char const* end, formatxx::format_spec& result)
{
	// sign
	if (start != end && *start == '+')
	{
		result.flags |= formatxx::format_flags::sign;
		++start;
	}
	else if (start != end && *start == ' ')
	{
		result.flags |= formatxx::format_flags::sign_space;
		++start;
	}
	else if (start != end && *start == '-')
	{
		// default flag
		++start;
	}

	// print numeric prefix
	if (start != end && *start == '#')
	{
		result.flags |= formatxx::format_flags::hash;
		++start;
	}

	// generic mode option allowed
	if (start != end && *start != ':' && *start != '}')
	{
		result.code = *start;
		++start;
	}

	// type-specific format allowed
	if (start != end && *start == ':')
	{
		char const* const extra = ++start;
		while (start != end && *start != '}' && *start != '{')
			++start;
		result.extra = formatxx::string_view(extra, start);
	}

	return start;
}

} // anonymous namespace
} // namespace formatxx