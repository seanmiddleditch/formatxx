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

#if !defined(_guard_FORMATXX_BUFFERED_H)
#define _guard_FORMATXX_BUFFERED_H
#pragma once

#include <formatxx/format.h>
#include <cstring>

namespace formatxx
{
	namespace _detail
	{
		template <typename T>
		struct new_delete_allocator
		{
			T* allocate(std::size_t count) { return new T[count]; }
			void deallocate(T* ptr, std::size_t) { delete[] ptr; }
		};

	} // namespace _detail

	template <typename CharT, std::size_t Size, typename AllocatorT = _detail::new_delete_allocator<CharT>> class basic_buffered_writer;

	template <std::size_t Size = 512, typename AllocatorT = _detail::new_delete_allocator<char>> using buffered_writer = basic_buffered_writer<char, Size, AllocatorT>;

} // namespace formatxx

/// A writer with a fixed buffer that will allocate when the buffer is exhausted.
template <typename CharT, std::size_t SizeN, typename AllocatorT>
class formatxx::basic_buffered_writer : public basic_format_writer<CharT>, private AllocatorT
{
public:
	basic_buffered_writer() = default;
	~basic_buffered_writer();

	basic_buffered_writer(basic_buffered_writer const&) = delete;
	basic_buffered_writer& operator=(basic_buffered_writer const&) = delete;

	void write(basic_string_view<CharT> str) override;

	void clear() { _last = _first; }
	std::size_t size() const { return _last - _first; }
	CharT const* c_str() const { return _first; }

private:
	void _grow(std::size_t amount);

	CharT* _first = _buffer;
	CharT* _last = _buffer;
	CharT* _sentinel = _buffer + SizeN;
	CharT _buffer[SizeN] = { CharT(0), };
};

template <typename CharT, std::size_t SizeN, typename AllocatorT>
formatxx::basic_buffered_writer<CharT, SizeN, AllocatorT>::~basic_buffered_writer()
{
	if (_first != _buffer)
		this->deallocate(_first, _sentinel - _first);
}

template <typename CharT, std::size_t SizeN, typename AllocatorT>
void formatxx::basic_buffered_writer<CharT, SizeN, AllocatorT>::_grow(std::size_t amount)
{
	std::size_t const size = _last - _first;
	std::size_t const capacity = _sentinel - _first;
	std::size_t const required = size + amount + 1;

	if (required > capacity) // need space for NUL byte
	{
		std::size_t newCapacity = capacity;
		newCapacity += newCapacity >> 1; // grow by 50%
		if (newCapacity < required) // ensure we get the space we asked for
			newCapacity = required;

		char* newBuffer = this->allocate(newCapacity);
		std::memcpy(newBuffer, _first, sizeof(CharT) * (size + 1));

		if (_first != _buffer)
			this->deallocate(_first, capacity);

		_first = newBuffer;
		_last = _first + size;
		_sentinel = _first + newCapacity;
	}
}

template <typename CharT, std::size_t SizeN, typename AllocatorT>
void formatxx::basic_buffered_writer<CharT, SizeN, AllocatorT>::write(basic_string_view<CharT> str)
{
	_grow(str.size());
	std::memcpy(_last, str.data(), sizeof(CharT) * str.size());
	_last += str.size();
	*_last = CharT(0);
}

#endif // !defined(_guard_FORMATXX_BUFFERED_H)
