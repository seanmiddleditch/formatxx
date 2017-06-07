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

void write_integer_prefix(format_writer& out, format_spec const& spec, bool negative);
template <typename T> void write_decimal(format_writer& out, T value);
template <typename T> void write_hexadecimal(format_writer& out, T value, bool lower);
template <typename T> void write_octal(format_writer& out, T value);
template <typename T> void write_binary(format_writer& out, T value);
template <typename T> void write_integer(format_writer& out, T value, string_view spec);


void write_integer_prefix(format_writer& out, format_spec const& spec, bool negative)
{
	char prefix_buffer[3]; // sign, type prefix
	char* prefix = prefix_buffer;

	// add sign
	if (negative)
		*(prefix++) = '-';
	else if (spec.sign == format_spec::sign_always)
		*(prefix++) = '+';
	else if (spec.sign == format_spec::sign_space)
		*(prefix++) = ' ';

	// add numeric type prefix
	if (spec.type_prefix)
	{
		*(prefix++) = '0';
		*(prefix++) = spec.code ? spec.code : 'd';
	}

	// write the prefix out, if any
	if (prefix != prefix_buffer)
		out.write({prefix_buffer, prefix});
}

template <typename T>
void write_decimal(format_writer& out, T value)
{
	// we'll work on every two decimal digits (groups of 100). notes taken from cppformat,
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

	// buffer must be one larger than digits10, as that trait is the maximum number of 
	// base-10 digits represented by the type in their entirety, e.g. 8-bits can store
	// 99 but not 999, so its digits10 is 2, even though the value 255 could be stored
	// and has 3 digits.
	char buffer[std::numeric_limits<decltype(value)>::digits10 + 1];
	char* end = buffer + sizeof(buffer);

	// work on every two decimal digits (groups of 100). notes taken from cppformat,
	// which took the notes from Alexandrescu from "Three Optimization Tips for C++"
	while (value >= 100)
	{
		// I feel like we could do the % and / better... somehow
		// we multiply the index by two to find the pair of digits to index
		unsigned const digit = (value % 100) << 1;
		value /= 100;

		// write out both digits of the given index
		*--end = sDecimalTable[digit + 1];
		*--end = sDecimalTable[digit];
	}

	if (value >= 10)
	{
		// we have two digits left; this is identical to the above loop, but without the division
		unsigned const digit = static_cast<unsigned>(value << 1);
		*--end = sDecimalTable[digit + 1];
		*--end = sDecimalTable[digit];
	}
	else
	{
		// we have but a single digit left, so this is easy
		*--end = '0' + static_cast<char>(value);
	}

	out.write({end, sizeof(buffer) - (end - buffer)});
}

template <typename T>
void write_hexadecimal(format_writer& out, T value, bool lower)
{
	char buffer[2 * sizeof(value)]; // 2 hex digits per octet
	char* end = buffer + sizeof(buffer);

	char const* const alphabet = lower ?
		"0123456789abcdef" :
		"0123456789ABCDEF";

	do
	{
		*--end = alphabet[value & 0xF];
	}
	while ((value >>= 4) != 0);

	out.write({end, sizeof(buffer) - (end - buffer)});
}

template <typename T>
void write_octal(format_writer& out, T value)
{
	char buffer[3 * sizeof(value)]; // 3 octal digits per octet
	char* end = buffer + sizeof(buffer);

	char const alphabet[] = "01234567";

	do
	{
		*--end = alphabet[value & 0x7];
	}
	while ((value >>= 3) != 0);

	out.write({end, sizeof(buffer) - (end - buffer)});
}

template <typename T>
void write_binary(format_writer& out, T value)
{
	char buffer[std::numeric_limits<unsigned char>::digits * sizeof(value)];
	char* end = buffer + sizeof(buffer);

	do
	{
		*--end = '0' + (value & 1);
	}
	while ((value >>= 1) != 0);

	out.write({end, sizeof(buffer) - (end - buffer)});
}

template <typename T>
void write_integer(format_writer& out, T raw, string_view spec_string)
{
	// subtract from 0 _after_ converting to deal with 2's complement format (abs(min) > abs(max))
	std::make_unsigned_t<T> const value = raw >= 0 ? raw : 0 - static_cast<std::make_unsigned_t<T>>(raw);

    format_spec const spec = parse_format_spec(spec_string);

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