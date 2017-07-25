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

#include "format_util.h"
#include <limits>

namespace formatxx {
namespace _detail {

template <typename CharT> unsigned write_integer_prefix(basic_format_writer<CharT>& out, basic_format_spec<CharT> const& spec, bool negative);
template <typename CharT, typename T> void write_decimal(basic_format_writer<CharT>& out, T value, unsigned padding, CharT pad);
template <typename CharT, typename T> void write_hexadecimal(basic_format_writer<CharT>& out, T value, bool lower);
template <typename CharT, typename T> void write_octal(basic_format_writer<CharT>& out, T value);
template <typename CharT, typename T> void write_binary(basic_format_writer<CharT>& out, T value);
template <typename CharT, typename T> void write_integer(basic_format_writer<CharT>& out, T value, basic_string_view<CharT> spec);

template <typename CharT>
unsigned write_integer_prefix(basic_format_writer<CharT>& out, basic_format_spec<CharT> const& spec, bool negative)
{
	CharT prefix_buffer[3]; // sign, type prefix
	CharT* prefix = prefix_buffer;

	// add sign
	if (negative)
	{
		*(prefix++) = FormatTraits<CharT>::cMinus;
	}
	else if (spec.prepend_sign)
	{
		*(prefix++) = FormatTraits<CharT>::cPlus;
	}
	else if (spec.prepend_space)
	{
		*(prefix++) = FormatTraits<CharT>::cSpace;
	}

	// add numeric type prefix
	if (spec.alternate_form)
	{
		*(prefix++) = FormatTraits<CharT>::to_digit(0);
		// FIXME: misbehaves for code 'i'
		*(prefix++) = spec.code ? spec.code : 'd';
	}

	// write the prefix out, if any
	if (prefix != prefix_buffer)
	{
		out.write({prefix_buffer, prefix});
	}

	// returns length of prefix written
	return static_cast<unsigned>(prefix - prefix_buffer);
}

struct decimal_helper
{
	// buffer must be one larger than digits10, as that trait is the maximum number of 
	// base-10 digits represented by the type in their entirety, e.g. 8-bits can store
	// 99 but not 999, so its digits10 is 2, even though the value 255 could be stored
	// and has 3 digits.
	template <typename UnsignedT>
	static constexpr std::size_t buffer_size = std::numeric_limits<UnsignedT>::digits10 + 1;

	template <typename CharT, typename UnsignedT>
	static basic_string_view<CharT> write(CharT(&buffer)[buffer_size<UnsignedT>], UnsignedT value)
	{
		// we'll work on every two decimal digits (groups of 100). notes taken from cppformat,
		// which took the notes from Alexandrescu from "Three Optimization Tips for C++"
		CharT const* const table = FormatTraits<CharT>::sDecimalPairs;

		CharT* end = buffer + buffer_size<UnsignedT>;

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

		return {end, buffer_size<UnsignedT> - (end - buffer)};
	}
};

template <bool LowerCase>
struct hexadecimal_helper
{
 	// 2 hex digits per octet
	template <typename UnsignedT>
	static constexpr std::size_t buffer_size = 2 * sizeof(UnsignedT);

	template <typename CharT, typename UnsignedT>
	static basic_string_view<CharT> write(CharT(&buffer)[buffer_size<UnsignedT>], UnsignedT value)
	{
		CharT* end = buffer + buffer_size<UnsignedT>;

		CharT const* const alphabet = LowerCase ?
			FormatTraits<CharT>::sHexadecimalLower :
			FormatTraits<CharT>::sHexadecimalUpper;

		do
		{
			*--end = alphabet[value & 0xF];
		}
		while ((value >>= 4) != 0);

		return {end, buffer_size<UnsignedT> - (end - buffer)};
	}
};

struct octal_helper
{
 	// up to three 3 octal digits per octet - FIXME is that right? I don't think that's right
	template <typename UnsignedT>
	static constexpr std::size_t buffer_size = 3 * sizeof(UnsignedT);

	template <typename CharT, typename UnsignedT>
	static basic_string_view<CharT> write(CharT(&buffer)[buffer_size<UnsignedT>], UnsignedT value)
	{
		CharT* end = buffer + buffer_size<UnsignedT>;

		// the octal alphabet is a subset of hexadecimal,
		// and doesn't depend on casing.
		CharT const* const alphabet = FormatTraits<CharT>::sHexadecimalLower;

		do
		{
			*--end = alphabet[value & 0x7];
		}
		while ((value >>= 3) != 0);

		return {end, buffer_size<UnsignedT> - (end - buffer)};
	}
};

struct binary_helper
{
	// one digit per bit of the input
	template <typename UnsignedT>
 	static constexpr std::size_t buffer_size = sizeof(UnsignedT) * CHAR_BIT;

	template <typename CharT, typename UnsignedT>
	static basic_string_view<CharT> write(CharT(&buffer)[buffer_size<UnsignedT>], UnsignedT value)
	{
		CharT* end = buffer + buffer_size<UnsignedT>;

		do
		{
			*--end = FormatTraits<CharT>::to_digit(value & 1);
		}
		while ((value >>= 1) != 0);

		return {end, buffer_size<UnsignedT> - (end - buffer)};
	}
};

template <typename HelperT, typename CharT, typename ValueT>
void write_integer_helper(basic_format_writer<CharT>& out, ValueT raw_value, basic_format_spec<CharT> const& spec)
{
	// convert to an unsigned value to make the formatting easier; note that must
	// subtract from 0 _after_ converting to deal with 2's complement format
	// where (abs(min) > abs(max)), otherwise we'd not be able to format -min<T>
	using unsigned_type = typename std::make_unsigned<ValueT>::type;
	unsigned_type const unsigned_value = raw_value >= 0 ? raw_value : 0 - static_cast<unsigned_type>(raw_value);

	// format any prefix onto the number
	unsigned const prefix_len = write_integer_prefix(out, spec, /*negative=*/raw_value < 0);

	std::size_t const padding = spec.has_width ? spec.width : (spec.has_precision && spec.precision > prefix_len) ? (spec.precision - prefix_len) : 0;
	CharT const pad_char = (spec.leading_zeroes || spec.has_precision) && !spec.left_justify ? '0' : ' ';

	constexpr std::size_t buffer_size = HelperT::buffer_size<typename unsigned_type>;
	CharT buffer[buffer_size];
	auto const result = HelperT::write(buffer, unsigned_value);

	write_padded_aligned(out, result, pad_char, padding, spec.left_justify);
}

template <typename CharT, typename T>
void write_integer(basic_format_writer<CharT>& out, T raw, basic_string_view<CharT> spec_string)
{
	basic_format_spec<CharT> const spec = parse_format_spec(spec_string);

	switch (spec.code)
	{
	default:
	case 0:
	case 'i':
	case 'd':
	case 'D':
		return write_integer_helper<decimal_helper>(out, raw, spec);
 	case 'x':
	 	return write_integer_helper<hexadecimal_helper</*lower=*/true>>(out, raw, spec);
 	case 'X':
	 	return write_integer_helper<hexadecimal_helper</*lower=*/false>>(out, raw, spec);
	case 'o':
	case 'O':
		return write_integer_helper<octal_helper>(out, raw, spec);
		break;
	case 'b':
	case 'B':
		return write_integer_helper<binary_helper>(out, raw, spec);
		break;
	}
}

} // namespace _detail
} // namespace formatxx

#endif // _guard_FORMATXX_DETAIL_WRITE_INTEGER_H
