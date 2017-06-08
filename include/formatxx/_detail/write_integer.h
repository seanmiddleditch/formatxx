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

#if !defined(_guard_FORMATXX_DETAIL_WRITE_INTEGER_H)
#define _guard_FORMATXX_DETAIL_WRITE_INTEGER_H
#pragma once

#include <limits>

namespace formatxx {
namespace _detail {
namespace {

template <typename CharT> void write_integer_prefix(basic_format_writer<CharT>& out, basic_format_spec<CharT> const& spec, bool negative);
template <typename CharT, typename T> void write_decimal(basic_format_writer<CharT>& out, T value);
template <typename CharT, typename T> void write_hexadecimal(basic_format_writer<CharT>& out, T value, bool lower);
template <typename CharT, typename T> void write_octal(basic_format_writer<CharT>& out, T value);
template <typename CharT, typename T> void write_binary(basic_format_writer<CharT>& out, T value);
template <typename CharT, typename T> void write_integer(basic_format_writer<CharT>& out, T value, basic_string_view<CharT> spec);

template <typename CharT>
void write_integer_prefix(basic_format_writer<CharT>& out, basic_format_spec<CharT> const& spec, bool negative)
{
	CharT prefix_buffer[3]; // sign, type prefix
	CharT* prefix = prefix_buffer;

	// add sign
	if (negative)
		*(prefix++) = FormatTraits<CharT>::cMinus;
	else if (spec.sign == FormatTraits<CharT>::cPlus || spec.sign == FormatTraits<CharT>::cSpace)
		*(prefix++) = spec.sign;

	// add numeric type prefix
	if (spec.type_prefix)
	{
		*(prefix++) = FormatTraits<CharT>::to_digit(0);
		*(prefix++) = spec.code ? spec.code : 'd';
	}

	// write the prefix out, if any
	if (prefix != prefix_buffer)
		out.write({prefix_buffer, prefix});
}

template <typename CharT, typename T>
void write_decimal(basic_format_writer<CharT>& out, T value)
{
	// we'll work on every two decimal digits (groups of 100). notes taken from cppformat,
	// which took the notes from Alexandrescu from "Three Optimization Tips for C++"
	CharT const* const table = FormatTraits<CharT>::sDecimalPairs;

	// buffer must be one larger than digits10, as that trait is the maximum number of 
	// base-10 digits represented by the type in their entirety, e.g. 8-bits can store
	// 99 but not 999, so its digits10 is 2, even though the value 255 could be stored
	// and has 3 digits.
	constexpr std::size_t buffer_size = std::numeric_limits<decltype(value)>::digits10 + 1;
	CharT buffer[buffer_size];
	CharT* end = buffer + buffer_size;

	// work on every two decimal digits (groups of 100). notes taken from cppformat,
	// which took the notes from Alexandrescu from "Three Optimization Tips for C++"
	while (value >= 100)
	{
		// I feel like we could do the % and / better... somehow
		// we multiply the index by two to find the pair of digits to index
		unsigned const digit = (value % 100) << 1;
		value /= 100;

		// write out both digits of the given index
		*--end = table[digit + 1];
		*--end = table[digit];
	}

	if (value >= 10)
	{
		// we have two digits left; this is identical to the above loop, but without the division
		unsigned const digit = static_cast<unsigned>(value << 1);
		*--end = table[digit + 1];
		*--end = table[digit];
	}
	else
	{
		// we have but a single digit left, so this is easy
		*--end = FormatTraits<CharT>::to_digit(static_cast<char>(value));
	}

	out.write({end, buffer_size - (end - buffer)});
}

template <typename CharT, typename T>
void write_hexadecimal(basic_format_writer<CharT>& out, T value, bool lower)
{
	CharT buffer[2 * sizeof(value)]; // 2 hex digits per octet
	CharT* end = buffer + sizeof(buffer);

	CharT const* const alphabet = lower ?
		FormatTraits<CharT>::sHexadecimalLower :
		FormatTraits<CharT>::sHexadecimalUpper;

	do
	{
		*--end = alphabet[value & 0xF];
	}
	while ((value >>= 4) != 0);

	out.write({end, sizeof(buffer) - (end - buffer)});
}

template <typename CharT, typename T>
void write_octal(basic_format_writer<CharT>& out, T value)
{
	CharT buffer[3 * sizeof(value)]; // 3 octal digits per octet
	CharT* end = buffer + sizeof(buffer);

	// the octal alphabet is a subset of hexadecimal,
	// and doesn't depend on casing.
	CharT const* const alphabet = FormatTraits<CharT>::sHexadecimalLower;

	do
	{
		*--end = alphabet[value & 0x7];
	}
	while ((value >>= 3) != 0);

	out.write({end, sizeof(buffer) - (end - buffer)});
}

template <typename CharT, typename T>
void write_binary(basic_format_writer<CharT>& out, T value)
{
	CharT buffer[std::numeric_limits<unsigned char>::digits * sizeof(value)];
	CharT* end = buffer + sizeof(buffer);

	do
	{
		*--end = FormatTraits<CharT>::to_digit(value & 1);
	}
	while ((value >>= 1) != 0);

	out.write({end, sizeof(buffer) - (end - buffer)});
}

template <typename CharT, typename T>
void write_integer(basic_format_writer<CharT>& out, T raw, basic_string_view<CharT> spec_string)
{
	// subtract from 0 _after_ converting to deal with 2's complement format (abs(min) > abs(max))
	std::make_unsigned_t<T> const value = raw >= 0 ? raw : 0 - static_cast<std::make_unsigned_t<T>>(raw);

    basic_format_spec<CharT> const spec = parse_format_spec(spec_string);

	// format any prefix onto the number
	write_integer_prefix(out, spec, /*negative=*/raw < 0);
	
	switch (spec.code)
	{
	case 0:
	case 'd':
	case 'D':
		write_decimal(out, value);
		break;
	case 'x':
		write_hexadecimal(out, value, /*lower=*/true);
		break;
	case 'X':
		write_hexadecimal(out, value, /*lower=*/false);
		break;
	case 'o':
	case 'O':
		write_octal(out, value);
		break;
	case 'b':
	case 'B':
		write_binary(out, value);
		break;
	}
}

} // anonymous namespace
} // namespace _detail
} // namespace formatxx

#endif // _guard_FORMATXX_DETAIL_WRITE_INTEGER_H
