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
#include "format_util.h"

namespace formatxx {

	template <typename CharT>
	FORMATXX_PUBLIC basic_parse_spec_result<CharT> FORMATXX_API parse_format_spec(basic_string_view<CharT> spec) noexcept {
		using Traits = _detail::FormatTraits<CharT>;

        basic_parse_spec_result<CharT> result;

        CharT const* start = spec.data();
		CharT const* const end = start + spec.size();

		// flags
		while (start != end) {
			if (*start == Traits::cPlus) {
				result.spec.prepend_sign = true;
			}
			else if (*start == Traits::cMinus) {
				result.spec.left_justify = true;
			}
			else if (*start == Traits::to_digit(0)) {
                result.spec.leading_zeroes = true;
			}
			else if (*start == Traits::cSpace) {
                result.spec.prepend_space = true;
			}
			else if (*start == Traits::cHash) {
                result.spec.alternate_form = true;
			}
			else {
				break;
			}
			++start;
		}

		// read in width
		start = _detail::parse_unsigned(start, end, result.spec.width);

		// read in precision, if present
		if (start != end && *start == Traits::cDot) {
            result.spec.has_precision = true;
			start = _detail::parse_unsigned(start + 1, end, result.spec.precision);
		}

		// read in any of the modifiers like h or l that modify a type code (no effect in our system)
		while (start != end && _detail::string_contains(Traits::sPrintfModifiers, *start)) {
			++start;
		}

		// generic code specified option allowed (required for printf)
		if (start != end && _detail::string_contains(Traits::sPrintfSpecifiers, *start)) {
            result.spec.code = *start++;
		}

        result.unparsed = { start, end };

		return result;
	}

} // namespace formatxx

#endif // _guard_FORMATXX_DETAIL_PARSE_FORMAT_H
