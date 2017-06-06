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

#include "write_integer.inl"

#include <cstdio>
#include <limits>
#include <climits>
#include <cinttypes>

namespace formatxx {

namespace {

template <typename CharT> struct FormatTraits;

template <> struct FormatTraits<char>
{
	static constexpr char cFormatBegin = '{';
	static constexpr char cFormatEnd = '}';
	static constexpr char cFormatSep = ':';

	static constexpr char cPrintfSpec = '%';

	// string_view over string literals is safe on all platforms for which I'm aware
	static constexpr string_view sErrBadFormat{"#BADF", 5};
	static constexpr string_view sErrIncomplete{"#INCL", 5};
	static constexpr string_view sErrOutOfRange{"#RNGE", 5};

	static constexpr string_view sTrue{"true", 4};
	static constexpr string_view sFalse{"false", 5};

	static constexpr string_view sPrintfSpecifiers{"bcsdioxXufFeEaAgGp", 18};
};

template <> struct FormatTraits<wchar_t>
{
	static constexpr wchar_t cFormatBegin = L'{';
	static constexpr wchar_t cFormatEnd = L'}';
	static constexpr wchar_t cFormatSep = L':';

	static constexpr wchar_t cPrintfSpec = L'%';

	// string_view over string literals is safe on all platforms for which I'm aware
	static constexpr wstring_view sErrBadFormat{L"#BADF", 5};
	static constexpr wstring_view sErrIncomplete{L"#INCL", 5};
	static constexpr wstring_view sErrOutOfRange{L"#RNGE", 5};

	static constexpr wstring_view sTrue{L"true", 4};
	static constexpr wstring_view sFalse{L"false", 5};

	static constexpr wstring_view sPrintfSpecifiers{L"bcsdioxXufFeEaAgGp", 18};
};

template <typename CharT> CharT const* parse_unsigned(CharT const* start, CharT const* end, unsigned& result);

} // anonymous namespace

void format_value(format_writer& out, char ch, string_view)
{
	out.write(string_view(&ch, 1));
}

void format_value(format_writer& out, bool value, string_view)
{
	out.write(value ? "true" : "false");
}

void format_value(format_writer& out, char* zstr, string_view spec)
{
	format_value(out, string_view(zstr), spec);
}

void format_value(format_writer& out, char const* zstr, string_view spec)
{
	format_value(out, string_view(zstr), spec);
}

void format_value(format_writer& out, string_view str, string_view)
{
	out.write(str);
}

void format_value(format_writer& out, signed int value, string_view spec) { write_integer(out, value, spec); }
void format_value(format_writer& out, signed char value, string_view spec) { write_integer(out, value, spec); }
void format_value(format_writer& out, signed long value, string_view spec) { write_integer(out, value, spec); }
void format_value(format_writer& out, signed short value, string_view spec) { write_integer(out, value, spec); }
void format_value(format_writer& out, signed long long value, string_view spec) { write_integer(out, value, spec); }

void format_value(format_writer& out, unsigned int value, string_view spec) { write_integer(out, value, spec); }
void format_value(format_writer& out, unsigned char value, string_view spec) { write_integer(out, value, spec); }
void format_value(format_writer& out, unsigned long value, string_view spec) { write_integer(out, value, spec); }
void format_value(format_writer& out, unsigned short value, string_view spec) { write_integer(out, value, spec); }
void format_value(format_writer& out, unsigned long long value, string_view spec) { write_integer(out, value, spec); }

void format_value(format_writer& out, float value, string_view spec)
{
	format_value(out, static_cast<double>(value), spec);
}

void format_value(format_writer& out, double value, string_view spec_string)
{
	char fmt[3] = "%f";

	format_spec const spec = parse_format_spec(spec_string);

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

void format_value(format_writer& out, void* ptr, string_view spec)
{
    write_integer(out, reinterpret_cast<std::uintptr_t>(ptr), spec);
}

void format_value(format_writer& out, void const* ptr, string_view spec)
{
    write_integer(out, reinterpret_cast<std::uintptr_t>(ptr), spec);
}

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

template <typename CharT>
basic_format_spec<CharT> parse_format_spec(basic_string_view<CharT> spec)
{
	basic_format_spec<CharT> result;

	if (spec.empty())
		return result;

	CharT const* start = spec.data();
	CharT const* const end = spec.data() + spec.size();

	// sign
	if (*start == '+')
	{
		result.sign = basic_format_spec<CharT>::sign_always;
		++start;
	}
	else if (*start == ' ')
	{
		result.sign = basic_format_spec<CharT>::sign_space;
		++start;
	}
	else if (*start == '-')
	{
		result.sign = basic_format_spec<CharT>::sign_default;
		++start;
	}

	if (start == end)
		return result;

	// print numeric prefix
	if (*start == '#')
	{
		result.type_prefix = true;
		
		if (++start == end)
			return result;
	}

	// generic code specified option allowed (required for printf)
	CharT const code = *start;
	for (CharT c : FormatTraits<CharT>::sPrintfSpecifiers)
	{
		if (code == c)
		{
			result.code = code;

			if (++start == end)
				return result;
		}
	}

	// store remaining format specifier
	result.extra = basic_string_view<CharT>(start, end);

	return result;
}

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
			CharT const* iter = parse_unsigned(start, end, index);

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

template <typename CharT>
basic_format_writer<CharT>& printf_impl(basic_format_writer<CharT>& out, basic_string_view<CharT> format, std::size_t count, BasicFormatterThunk<CharT> const* funcs, FormatterParameter const* values)
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

template basic_format_writer<char>& format_impl(basic_format_writer<char>& out, basic_string_view<char> format, std::size_t count, BasicFormatterThunk<char> const* funcs, FormatterParameter const* values);
template basic_format_writer<wchar_t>& format_impl(basic_format_writer<wchar_t>& out, basic_string_view<wchar_t> format, std::size_t count, BasicFormatterThunk<wchar_t> const* funcs, FormatterParameter const* values);

template basic_format_writer<char>& printf_impl(basic_format_writer<char>& out, basic_string_view<char> format, std::size_t count, BasicFormatterThunk<char> const* funcs, FormatterParameter const* values);
template basic_format_writer<wchar_t>& printf_impl(basic_format_writer<wchar_t>& out, basic_string_view<wchar_t> format, std::size_t count, BasicFormatterThunk<wchar_t> const* funcs, FormatterParameter const* values);

} // namespace _detail

template basic_format_spec<char> parse_format_spec(basic_string_view<char>);
template basic_format_spec<wchar_t> parse_format_spec(basic_string_view<wchar_t>);

namespace {

template <typename CharT>
CharT const* parse_unsigned(CharT const* start, CharT const* end, unsigned& result)
{
	result = 0;
	while (start != end && *start >= CharT('0') && *start <= CharT('9'))
	{
		result *= 10;
		result += *start - CharT('0');
		++start;
	}
	return start;
}

} // anonymous namespace
} // namespace formatxx