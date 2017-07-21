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

#if !defined(_guard_FORMATXX_DETAIL_PARSE_FORMAT_H)
#define _guard_FORMATXX_DETAIL_PARSE_FORMAT_H
#pragma once

#include "parse_unsigned.h"

namespace formatxx {
namespace _detail {

template <typename CharT>
bool contains(basic_string_view<CharT> haystack, CharT needle)
{
	for (CharT const c : haystack)
	{
		if (c == needle)
		{
			return true;
		}
	}
	return false;
}

}

template <typename CharT>
FORMATXX_PUBLIC basic_format_spec<CharT> FORMATXX_API parse_format_spec(basic_string_view<CharT> spec)
{
	using Traits = _detail::FormatTraits<CharT>;

	basic_format_spec<CharT> result;
	// if we early-out, this will ensure that the extra points to the end of th einput
	// FIXME: this is terrible API requirement all around
	result.extra = {spec.end(), spec.end()};

	if (spec.empty())
	{
		return result;
	}

	CharT const* start = spec.data();
	CharT const* const end = spec.data() + spec.size();

	// flags
	for (;;)
	{
		if (start == end)
		{
			return result; // incomplete, so error
		}
		else if (*start == Traits::cPlus ||
			*start == Traits::cMinus)
		{
			result.sign = *start;
		}
		else if (*start == Traits::to_digit(0))
		{
			result.pad = *start; // zero-pad numbers
		}
		else if (*start == Traits::cSpace)
		{
			result.sign = *start; // numbers get a space for positive numbers
			result.pad = *start; // strings get padded with spaces
		}
		else if (*start == Traits::cHash)
		{
			// "alternate form"
			result.type_prefix = true;
		}
		else
		{
			break;
		}
		++start;
	}

	// read in width
	start = _detail::parse_unsigned(start, end, result.width);
	if (start == end)
	{
		return result;
	}

	// read in precision, if present
	if (*start == Traits::cDot)
	{
		start = _detail::parse_unsigned(start + 1, end, result.precision);
		if (start == end)
		{
			return result;
		}
	}

	// read in any of the modifiers like h or l that modify a type code (no effect in our system)
	while (_detail::contains(Traits::sPrintfModifiers, *start))
	{
		if (++start == end)
		{
			return result;
		}
	}


	// generic code specified option allowed (required for printf)
	if (_detail::contains(Traits::sPrintfSpecifiers, *start))
	{
		result.code = *start++;
	}

	// store remaining format specifier
	result.extra = basic_string_view<CharT>(start, end);

	return result;
}

} // namespace formatxx

#endif // _guard_FORMATXX_DETAIL_PARSE_FORMAT_H
