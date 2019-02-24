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

#if !defined(_guard_FORMATXX_FIXED_H)
#define _guard_FORMATXX_FIXED_H
#pragma once

#include <cstring> // for std::memcpy

namespace formatxx
{
	template <typename CharT, std::size_t> class basic_fixed_writer;

	template <std::size_t Size = 512> using fixed_writer = basic_fixed_writer<char, Size>;
	template <std::size_t Size = 512> using wfixed_writer = basic_fixed_writer<wchar_t, Size>;

}

/// A writer with a fixed buffer that will never allocate.
template <typename CharT, std::size_t SizeN>
class formatxx::basic_fixed_writer : public basic_format_writer<CharT>
{
public:
	void write(basic_string_view<CharT> str) override;

	void clear() { _last = _buffer; }
	std::size_t size() const { return _last - _buffer; }
	CharT const* c_str() const { return _buffer; }

private:
	CharT* _last = _buffer;
	CharT _buffer[SizeN] = { CharT(0), };
};

template <typename CharT, std::size_t SizeN>
void formatxx::basic_fixed_writer<CharT, SizeN>::write(basic_string_view<CharT> str)
{
	std::size_t const remaining = SizeN - size() - 1;
	std::size_t const length = remaining < str.size() ? remaining : str.size();
	std::memcpy(_last, str.data(), length * sizeof(CharT));
	_last += length;
	*_last = CharT(0);
}

#endif // !defined(_guard_FORMATXX_FIXED_H)
