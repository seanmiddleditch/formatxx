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

namespace formatxx {

template <typename CharT>
FORMATXX_PUBLIC basic_format_spec<CharT> FORMATXX_API parse_format_spec(basic_string_view<CharT> spec)
{
	basic_format_spec<CharT> result;

	if (spec.empty())
		return result;

	CharT const* start = spec.data();
	CharT const* const end = spec.data() + spec.size();

	// sign
	if (*start == _detail::FormatTraits<CharT>::cSpecPlus)
	{
		result.sign = basic_format_spec<CharT>::sign_always;
		++start;
	}
	else if (*start == _detail::FormatTraits<CharT>::cSpecSpace)
	{
		result.sign = basic_format_spec<CharT>::sign_space;
		++start;
	}
	else if (*start == _detail::FormatTraits<CharT>::cSpecMinus)
	{
		result.sign = basic_format_spec<CharT>::sign_default;
		++start;
	}

	if (start == end)
		return result;

	// print numeric prefix
	if (*start == _detail::FormatTraits<CharT>::cSpecHash)
	{
		result.type_prefix = true;
		
		if (++start == end)
			return result;
	}

	// generic code specified option allowed (required for printf)
	CharT const code = *start;
	for (CharT c : _detail::FormatTraits<CharT>::sPrintfSpecifiers)
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

} // namespace formatxx

#endif // _guard_FORMATXX_DETAIL_PARSE_FORMAT_H
