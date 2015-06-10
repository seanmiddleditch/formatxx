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
#include <cstring>

namespace std
{
	template <typename, typename, typename> class basic_string;
	template <typename> class allocator;
}

namespace fmt2
{
	struct StringView;
	struct FormatSpec;

	class IWriter;
	template <typename>	class StringWriter;
	template <std::size_t> class FixedWriter;
	template <std::size_t, typename> class BufferedWriter;

	template <typename StringT, typename... Args> StringT FormatString(StringView format, Args&&... args);

	template <typename T> void format(IWriter&, T const&, FormatSpec const&);
}

/// \brief Describes a format string.
struct fmt2::StringView
{
	char const* begin = nullptr;
	char const* end = nullptr;

	StringView(char const* str) : begin(str), end(str + std::strlen(str)) {}

	template <typename TraitsT, typename AllocatorT>
	StringView(std::basic_string<char, TraitsT, AllocatorT> const& str) : begin(str.c_str()), end(str.c_str() + str.size()) {}

	// hmm, this may be a bad idea, it'll bind to over-long character buffers
	template <size_t N> StringView(char (&str)[N]) : begin(str), end(str + N) {}
};

/// \brief Interface for any buffer that the format library can write into.
class fmt2::IWriter
{
public:
	virtual ~IWriter() = default;

	virtual void Write(char const* nstr, std::size_t length) = 0;
	virtual void Write(char const* zstr) = 0;

	template <typename... Args>
	void Format(StringView format, Args&&... args);
};

/// \brief A writer that generates a buffer (intended for std::string).
template <typename StringT = std::string>
class fmt2::StringWriter : public IWriter
{
	StringT _string;

public:
	void Write(char const* nstr, std::size_t length) override { _string.append(nstr, length); }
	void Write(char const* zstr) override { _string.append(zstr); }

	StringT const& String() const& { return _string; }
	StringT&& String() && { return std::move(_string); }

	std::size_t size() const { return _string.size(); }
	char const* c_str() const { return _string.c_str(); }
};

/// \brief A writer with a fixed buffer that will never allocate.
template <std::size_t SizeN = 512>
class fmt2::FixedWriter : public IWriter
{
	std::size_t _length = 0;
	char _buffer[SizeN] = {'\0',};

public:
	void Write(char const* nstr, size_t length) override;
	void Write(char const* zstr) override;

	std::size_t size() const { return _length; }
	char const* c_str() const { return _buffer; }
};

/// \brief A writer with a fixed buffer that will allocate when the buffer is exhausted.
template <std::size_t SizeN = 256, typename AllocatorT = std::allocator<char>>
class fmt2::BufferedWriter : public IWriter, private AllocatorT
{
	std::size_t _length = 0;
	std::size_t _capacity = SizeN;
	char* _buffer = _fixed;
	char _fixed[SizeN] = {'\0',};

	void Ensure(std::size_t amount);

public:
	BufferedWriter() = default;
	~BufferedWriter();

	BufferedWriter(BufferedWriter const&) = delete;
	BufferedWriter& operator=(BufferedWriter const&) = delete;

	void Write(char const* nstr, size_t length) override;
	void Write(char const* zstr) override { Write(zstr, std::strlen(zstr)); }

	std::size_t size() const { return _length; }
	char const* c_str() const { return _buffer; }
};

/// \brief Extra formatting specifications.
struct fmt2::FormatSpec
{
	unsigned width = 0;
	unsigned precision = 0;
	// #FIXME: alignment
	// #FIXME: flags
	// #FIXME: custom string part
};

namespace fmt2
{
	/// \brief Format interface to overload for custom types.
	template <typename T> void format(IWriter& writer, T const& value, FormatSpec const& spec) = delete;

	/// \brief Default format helpers.
	void format(IWriter& writer, char const* zstr, FormatSpec const& spec);
	void format(IWriter& writer, StringView str, FormatSpec const& spec);
	void format(IWriter& writer, char ch, FormatSpec const& spec);
	void format(IWriter& writer, bool value, FormatSpec const& spec);
	void format(IWriter& writer, float value, FormatSpec const& spec);
	void format(IWriter& writer, signed int value, FormatSpec const& spec);
	void format(IWriter& writer, signed long value, FormatSpec const& spec);
	void format(IWriter& writer, signed short value, FormatSpec const& spec);
	void format(IWriter& writer, signed long long value, FormatSpec const& spec);
	void format(IWriter& writer, unsigned int value, FormatSpec const& spec);
	void format(IWriter& writer, unsigned long value, FormatSpec const& spec);
	void format(IWriter& writer, unsigned short value, FormatSpec const& spec);
	void format(IWriter& writer, unsigned long long value, FormatSpec const& spec);

	template <typename TraitsT, typename AllocatorT>
	void format(IWriter& writer, std::basic_string<char, TraitsT, AllocatorT> const& string, FormatSpec const& spec);

	/// \internal
	namespace _detail
	{
		using FormatFunc = void(*)(IWriter&, void const*, FormatSpec const&);

		template <typename T> struct wrap { static void fwd(IWriter& writer, void const* ptr, FormatSpec const& spec) { format(writer, *static_cast<T const*>(ptr), spec); } };
		// #FIXME: char[N] types will horribly do the wrong thing here.

		void format_impl(IWriter& writer, StringView format, std::size_t count, FormatFunc const* funcs, void const** values);
	}
}

/// \brief Write the string format using the given parameters into a buffer.
/// \param format The primary text and formatting controls to be written.
/// \param args The arguments used by the formatting string.
template <typename... Args>
void fmt2::IWriter::Format(StringView format, Args&&... args)
{
	void const* values[] = {std::addressof(args)..., nullptr};
	constexpr _detail::FormatFunc funcs[] = {&_detail::wrap<std::decay_t<Args>>::fwd..., nullptr};

	_detail::format_impl(*this, format, sizeof...(Args), funcs, values);
}

template <typename StringT = std::string, typename... Args>
StringT fmt2::FormatString(StringView format, Args&&... args)
{
	StringWriter<StringT> writer;
	writer.Format(format, args...);
	return std::move(writer).String();
}

template <typename TraitsT, typename AllocatorT>
void fmt2::format(IWriter& writer, std::basic_string<char, TraitsT, AllocatorT> const& string, FormatSpec const&)
{
	writer.Write(string.c_str(), string.size());
}

template <std::size_t SizeN>
void fmt2::FixedWriter<SizeN>::Write(char const* nstr, std::size_t length)
{
	while (length > 0 && _length < SizeN-1)
		_buffer[_length++] = *(nstr++);
	_buffer[_length] = '\0';
}

template <std::size_t SizeN>
void fmt2::FixedWriter<SizeN>::Write(char const* zstr)
{
	while (_length < SizeN-1 && *zstr != '\0')
		_buffer[_length++] = *(zstr++);
	_buffer[_length] = '\0';
}

template <std::size_t SizeN, typename AllocatorT>
fmt2::BufferedWriter<SizeN, AllocatorT>::~BufferedWriter()
{
	if (_buffer != _fixed)
		this->deallocate(_buffer, _capacity);
}

template <std::size_t SizeN, typename AllocatorT>
void fmt2::BufferedWriter<SizeN, AllocatorT>::Ensure(std::size_t amount)
{
	size_t const required = _length + amount + 1;
	if (required > _capacity) // need space for NUL byte
	{
		size_t newCapacity = _capacity;
		newCapacity += newCapacity >> 1; // grow by 50%
		if (newCapacity < required) // ensure we get the space we asked for
			newCapacity = required;

		char* newBuffer = this->allocate(newCapacity);
		std::memcpy(newBuffer, _buffer, _length + 1);

		if (_buffer != _fixed)
			this->deallocate(_buffer, _capacity);

		_buffer = newBuffer;
		_capacity = newCapacity;
	}
}

template <std::size_t SizeN, typename AllocatorT>
void fmt2::BufferedWriter<SizeN, AllocatorT>::Write(char const* nstr, std::size_t length)
{
	Ensure(length);
	std::memcpy(_buffer + _length, nstr, length);
	_length += length;
	_buffer[_length] = '\0';
}

#endif // !defined(_guard_FORMATXX_H)
