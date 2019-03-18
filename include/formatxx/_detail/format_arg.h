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

#if !defined(_guard_FORMATXX_DETAIL_FORMAT_ARG_H)
#define _guard_FORMATXX_DETAIL_FORMAT_ARG_H
#pragma once

#include <formatxx/_detail/write_integer.h>
#include <formatxx/_detail/write_string.h>
#include <formatxx/_detail/write_float.h>
#include <formatxx/_detail/write_wide.h>

#include <cinttypes>

template <typename CharT>
formatxx::result_code FORMATXX_API formatxx::basic_format_arg<CharT>::format_into(basic_format_writer<CharT>& output, basic_string_view<CharT> spec) const {
    switch (_type) {
    case arg_type::char_t:
        _detail::write_char(output, *static_cast<char const*>(_value), spec);
        return result_code::success;
    case arg_type::wchar:
        _detail::write_char(output, *static_cast<wchar_t const*>(_value), spec);
        return result_code::success;
    case arg_type::signed_char:
        _detail::write_integer(output, *static_cast<signed char const*>(_value), spec);
        return result_code::success;
    case arg_type::unsigned_char:
        _detail::write_integer(output, *static_cast<unsigned char const*>(_value), spec);
        return result_code::success;
    case arg_type::signed_short_int:
		_detail::write_integer(output, *static_cast<signed short const*>(_value), spec);
		return result_code::success;
    case arg_type::unsigned_short_int:
		_detail::write_integer(output, *static_cast<unsigned short const*>(_value), spec);
		return result_code::success;
    case arg_type::signed_long_int:
		_detail::write_integer(output, *static_cast<signed long const*>(_value), spec);
		return result_code::success;
    case arg_type::unsigned_long_int:
		_detail::write_integer(output, *static_cast<unsigned long const*>(_value), spec);
		return result_code::success;
    case arg_type::signed_long_long_int:
		_detail::write_integer(output, *static_cast<signed long long const*>(_value), spec);
		return result_code::success;
    case arg_type::unsigned_long_long_int:
		_detail::write_integer(output, *static_cast<unsigned long long const*>(_value), spec);
		return result_code::success;
    case arg_type::single_float:
		_detail::write_float(output, *static_cast<float const*>(_value), spec);
		return result_code::success;
    case arg_type::double_float:
		_detail::write_float(output, *static_cast<double const*>(_value), spec);
		return result_code::success;
    case arg_type::boolean:
		_detail::write_string(output, *static_cast<bool const*>(_value) ? _detail::FormatTraits<CharT>::sTrue : _detail::FormatTraits<CharT>::sFalse, spec);
		return result_code::success;
    case arg_type::char_string:
		_detail::write_string(output, string_view(*static_cast<char const* const*>(_value)), spec);
		return result_code::success;
    case arg_type::wchar_string:
		_detail::write_string(output, wstring_view(*static_cast<wchar_t const* const*>(_value)), spec);
		return result_code::success;
    case arg_type::null_pointer:
		_detail::write_string(output, _detail::FormatTraits<CharT>::sNullptr, spec);
		return result_code::success;
    case arg_type::void_pointer:
		_detail::write_integer(output, reinterpret_cast<std::uintptr_t>(*static_cast<void const* const*>(_value)), spec);
		return result_code::success;
    case arg_type::custom:
        return _thunk(output, _value, spec);
    default:
        return result_code::success;
    }
}

#endif // !defined(_guard_FORMATXX_DETAIL_FORMAT_ARG_H)
