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

#if !defined(_guard_FORMATXX_H)
#define _guard_FORMATXX_H
#pragma once

#include <type_traits>
#include <litexx/string_view.h>
#include "formatxx/_detail/append_writer.h"

#if !defined(FORMATXX_API)
#	if defined(_WIN32)
#		define FORMATXX_API __stdcall
#	else
#		define FORMATXX_API
#	endif
#endif

#if defined(_WIN32) && !defined(FORMATXX_PUBLIC)
#	if defined(FORMATXX_EXPORT)
#		define FORMATXX_PUBLIC __declspec(dllexport)
#	else
#		define FORMATXX_PUBLIC
#	endif
#elif __GNUC__ >= 4 && !defined(FORMATXX_PUBLIC)
#	if defined(FORMATXX_EXPORT)
#		define FORMATXX_PUBLIC __attribute__((visibility("default")))
#	else
#		define FORMATXX_PUBLIC
#	endif
#endif

namespace formatxx {
    template <typename CharT> using basic_string_view = litexx::basic_string_view<CharT>;
    template <typename CharT> class basic_format_writer;
    template <typename CharT> class basic_format_spec;

    enum class result_code;

    using string_view = basic_string_view<char>;
    using format_writer = basic_format_writer<char>;
    using format_spec = basic_format_spec<char>;

    using wstring_view = basic_string_view<wchar_t>;
    using wformat_writer = basic_format_writer<wchar_t>;
    using wformat_spec = basic_format_spec<wchar_t>;

    template <typename CharT, typename FormatT, typename... Args> result_code format_to(basic_format_writer<CharT>& writer, FormatT const& format, Args const& ... args);
    template <typename CharT, typename FormatT, typename... Args> result_code printf_to(basic_format_writer<CharT>& writer, FormatT const& format, Args const& ... args);

    template <typename ResultT, typename FormatT, typename... Args> ResultT format_as(FormatT const& format, Args const& ... args);
    template <typename ResultT, typename FormatT, typename... Args> ResultT printf_as(FormatT const& format, Args const& ... args);

    template <typename CharT> FORMATXX_PUBLIC basic_format_spec<CharT> FORMATXX_API parse_format_spec(basic_string_view<CharT> spec) noexcept;
}

namespace formatxx::_detail {
    enum class format_arg_type;

    template <typename CharT> class basic_format_arg;
    template <typename CharT> class basic_format_arg_list;

    template <typename CharT, typename T> constexpr basic_format_arg<CharT> make_format_arg(T const& value) noexcept;
}

enum class formatxx::result_code {
    success,
    out_of_range,
    malformed_input,
    out_of_space,
};

/// Interface for any buffer that the format library can write into.
template <typename CharT>
class formatxx::basic_format_writer {
public:
    virtual ~basic_format_writer() = default;

    /// Write a string slice.
    /// @param str The string to write.
    virtual void write(basic_string_view<CharT> str) = 0;
};

/// Extra formatting specifications.
template <typename CharT>
class formatxx::basic_format_spec {
public:
    CharT const* remaining = nullptr;
    unsigned width = 0;
    unsigned precision = 0;
    CharT code = 0;
    bool has_precision = false;
    bool left_justify = false;
    bool prepend_sign = false;
    bool prepend_space = false;
    bool alternate_form = false;
    bool leading_zeroes = false;
};

enum class formatxx::_detail::format_arg_type {
    unknown,
    char_t,
    wchar,
    signed_char,
    unsigned_char,
    signed_int,
    unsigned_int,
    signed_short_int,
    unsigned_short_int,
    signed_long_int,
    unsigned_long_int,
    signed_long_long_int,
    unsigned_long_long_int,
    single_float,
    double_float,
    boolean,
    char_string,
    wchar_string,
    null_pointer,
    void_pointer,
    custom
};

/// Abstraction for a single formattable value
template <typename CharT> 
class formatxx::_detail::basic_format_arg {
public:
    using thunk_type = result_code(FORMATXX_API*)(basic_format_writer<CharT>&, void const*, basic_string_view<CharT>);

    constexpr basic_format_arg() noexcept = default;
    constexpr basic_format_arg(_detail::format_arg_type type, void const* value) noexcept : _type(type), _value(value) {}
    constexpr basic_format_arg(thunk_type thunk, void const* value) noexcept : _type(_detail::format_arg_type::custom), _thunk(thunk), _value(value) {}

    FORMATXX_PUBLIC result_code FORMATXX_API format_into(basic_format_writer<CharT>& output, basic_string_view<CharT> spec) const;

private:
    _detail::format_arg_type _type = _detail::format_arg_type::unknown;
    thunk_type _thunk = nullptr;
    void const* _value = nullptr;
};

/// Abstraction for a set of format arguments.
template <typename CharT>
class formatxx::_detail::basic_format_arg_list {
public:
    using format_arg_type = basic_format_arg<CharT>;
    using thunk_type = typename format_arg_type::thunk_type;
    using size_type = std::size_t;

    constexpr basic_format_arg_list() noexcept = default;
    explicit constexpr basic_format_arg_list(size_type count, format_arg_type const* args) noexcept : _args(args), _count(count) {}

    constexpr result_code format_arg(basic_format_writer<CharT>& output, size_type index, basic_string_view<CharT> spec) const {
        return index < _count ? _args[index].format_into(output, spec) : result_code::out_of_range;
    }

private:
    basic_format_arg<CharT> const* _args = nullptr;
    size_type _count = 0;
};

namespace formatxx {
    /// Default format helpers.
    FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, string_view str, string_view spec = {}) noexcept;
    FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, wstring_view str, string_view spec = {}) noexcept;
    FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, string_view str, wstring_view spec = {}) noexcept;
    FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, wstring_view str, wstring_view spec = {}) noexcept;

    template <typename CharT, typename T>
    void FORMATXX_API format_value_to(basic_format_writer<CharT>& writer, T const& value, basic_string_view<CharT> spec = {}) {
        auto arg = _detail::make_format_arg<CharT>(value);
        arg.format_into(writer, spec);
    }
}

/// @internal
namespace formatxx::_detail {
    template <typename T>
    using remove_array = std::conditional_t<std::is_array_v<T>, std::remove_extent_t<T> const*, T>;

    template <typename C, typename T, typename V = void>
    struct has_format_value { static constexpr bool value = false; };
    template <typename C, typename T>
    struct has_format_value<C, T, std::void_t<decltype(format_value(std::declval<basic_format_writer<C>&>(), std::declval<T>(), std::declval<basic_string_view<C>>()))>> {
        static constexpr bool value = true;
    };

    template <typename T> struct type_of { static constexpr format_arg_type value = format_arg_type::unknown; };
#define FORMATXX_TYPE(x, e) template <> struct type_of<x> { static constexpr format_arg_type value = format_arg_type::e; };
    FORMATXX_TYPE(char, char_t);
    FORMATXX_TYPE(wchar_t, wchar);
    FORMATXX_TYPE(signed char, signed_char);
    FORMATXX_TYPE(unsigned char, unsigned_char);
    FORMATXX_TYPE(signed int, signed_int);
    FORMATXX_TYPE(unsigned int, unsigned_int);
    FORMATXX_TYPE(signed short, signed_short_int);
    FORMATXX_TYPE(unsigned short, unsigned_short_int);
    FORMATXX_TYPE(signed long, signed_long_int);
    FORMATXX_TYPE(unsigned long, unsigned_long_int);
    FORMATXX_TYPE(signed long long, signed_long_long_int);
    FORMATXX_TYPE(unsigned long long, unsigned_long_long_int);
    FORMATXX_TYPE(float, single_float);
    FORMATXX_TYPE(double, double_float);
    FORMATXX_TYPE(bool, boolean);
    FORMATXX_TYPE(char*, char_string);
    FORMATXX_TYPE(char const*, char_string);
    FORMATXX_TYPE(wchar_t*, char_string);
    FORMATXX_TYPE(wchar_t const*, wchar_string);
    FORMATXX_TYPE(std::nullptr_t, null_pointer);
    FORMATXX_TYPE(void*, void_pointer);
    FORMATXX_TYPE(void const*, void_pointer);
#undef FORMTAXX_TYPE

    template <typename CharT, typename T>
    result_code FORMATXX_API format_value_thunk(basic_format_writer<CharT>& out, void const* ptr, basic_string_view<CharT> spec) {
        format_value(out, *static_cast<T const*>(ptr), spec);
        return result_code::success;
    }

    template <typename CharT, typename T>
    constexpr basic_format_arg<CharT> make_format_arg(T const& value) noexcept {
        if constexpr (constexpr format_arg_type type = type_of<std::decay_t<T>>::value; type != format_arg_type::unknown) {
            return { type, &value };
        }
        else if constexpr (has_format_value<CharT, T>::value) {
            return basic_format_arg<CharT>(&format_value_thunk<CharT, T>, &value);
        }
        else if constexpr (std::is_pointer_v<T>) {
            return { format_arg_type::void_pointer, &value };
        }
        else if constexpr (std::is_enum_v<T>) {
            return { type_of<std::underlying_type_t<T>>::value, &value };
        }
        else {
            return {};
        }
    }

    template <typename CharT>
    FORMATXX_PUBLIC result_code FORMATXX_API format_impl(basic_format_writer<CharT>& out, basic_string_view<CharT> format, basic_format_arg_list<CharT> args);
    template <typename CharT>
    FORMATXX_PUBLIC result_code FORMATXX_API printf_impl(basic_format_writer<CharT>& out, basic_string_view<CharT> format, basic_format_arg_list<CharT> args);
}

extern template FORMATXX_PUBLIC formatxx::result_code FORMATXX_API formatxx::_detail::format_impl(basic_format_writer<char>& out, basic_string_view<char> format, basic_format_arg_list<char> args);
extern template FORMATXX_PUBLIC formatxx::result_code FORMATXX_API formatxx::_detail::printf_impl(basic_format_writer<char>& out, basic_string_view<char> format, basic_format_arg_list<char> args);
extern template FORMATXX_PUBLIC formatxx::result_code FORMATXX_API formatxx::_detail::basic_format_arg<char>::format_into(basic_format_writer<char>& output, basic_string_view<char> spec) const;
extern template FORMATXX_PUBLIC formatxx::basic_format_spec<char> FORMATXX_API formatxx::parse_format_spec(basic_string_view<char> spec) noexcept;

extern template FORMATXX_PUBLIC formatxx::result_code FORMATXX_API formatxx::_detail::format_impl(basic_format_writer<wchar_t>& out, basic_string_view<wchar_t> format, basic_format_arg_list<wchar_t> args);
extern template FORMATXX_PUBLIC formatxx::result_code FORMATXX_API formatxx::_detail::printf_impl(basic_format_writer<wchar_t>& out, basic_string_view<wchar_t> format, basic_format_arg_list<wchar_t> args);
extern template FORMATXX_PUBLIC formatxx::result_code FORMATXX_API formatxx::_detail::basic_format_arg<wchar_t>::format_into(basic_format_writer<wchar_t>& output, basic_string_view<wchar_t> spec) const;
extern template FORMATXX_PUBLIC formatxx::basic_format_spec<wchar_t> FORMATXX_API formatxx::parse_format_spec(basic_string_view<wchar_t> spec) noexcept;

/// Write the string format using the given parameters into a buffer.
/// @param writer The write buffer that will receive the formatted text.
/// @param format The primary text and formatting controls to be written.
/// @param args The arguments used by the formatting string.
template <typename CharT, typename FormatT, typename... Args>
formatxx::result_code formatxx::format_to(basic_format_writer<CharT>& writer, FormatT const& format, Args const& ... args) {
    _detail::basic_format_arg<CharT> format_args[] = { _detail::make_format_arg<CharT>(static_cast<_detail::remove_array<Args> const&>(args))... };
    return _detail::format_impl(writer, basic_string_view<CharT>(format), _detail::basic_format_arg_list<CharT>(sizeof...(args), format_args));
}

/// Write the printf format using the given parameters into a buffer.
/// @param writer The write buffer that will receive the formatted text.
/// @param format The primary text and printf controls to be written.
/// @param args The arguments used by the formatting string.
template <typename CharT, typename FormatT, typename... Args>
formatxx::result_code formatxx::printf_to(basic_format_writer<CharT>& writer, FormatT const& format, Args const& ... args) {
    _detail::basic_format_arg<CharT> format_args[] = { _detail::make_format_arg<CharT>(static_cast<_detail::remove_array<Args> const&>(args))... };
    return _detail::printf_impl(writer, basic_string_view<CharT>(format), _detail::basic_format_arg_list<CharT>(sizeof...(args), format_args));
}

/// Write the string format using the given parameters and return a string with the result.
/// @param format The primary text and formatting controls to be written.
/// @param args The arguments used by the formatting string.
/// @returns a formatted string.
template <typename ResultT, typename FormatT, typename... Args>
ResultT formatxx::format_as(FormatT const& format, Args const& ... args) {
    ResultT result;
    append_writer writer(result);
    formatxx::format_to(writer, basic_string_view<typename ResultT::value_type>(format), args...);
    return result;
}

/// Write the printf format using the given parameters and return a string with the result.
/// @param format The primary text and printf controls to be written.
/// @param args The arguments used by the formatting string.
template <typename ResultT, typename FormatT, typename... Args>
ResultT formatxx::printf_as(FormatT const& format, Args const& ... args) {
    ResultT result;
    append_writer writer(result);
    formatxx::printf_to(writer, basic_string_view<typename ResultT::value_type>(format), args...);
    return result;
}

#endif // !defined(_guard_FORMATXX_H)
