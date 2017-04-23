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

#include <formatxx/format.h>

#include <iostream>
#include <string>
#include <cstdint>
#include <limits>

static int formatxx_tests = 0;
static int formatxx_failed = 0;

std::string formatxx_tostring(std::string&& str) { return std::move(str); }
std::string formatxx_tostring(formatxx::format_writer const& writer) { return std::string(writer.view().data(), writer.view().size()); }

#define CHECK_FORMAT_HELPER(expr, expected) \
	do{ \
		++formatxx_tests; \
		auto const& result = formatxx_tostring((expr)); \
		if (result == (expected)) {} else { \
			std::cerr << __FILE__ << '(' << __LINE__ << "): TEST FAILED\n"; \
			std::cerr << #expr << '\n'; \
			std::cerr << "  Expected: " << (expected) << '\n'; \
			std::cerr << "  Received: " << result << '\n'; \
			++formatxx_failed; \
		} \
	}while(false)


#define CHECK_FORMAT(expected, arg0, ...) \
	CHECK_FORMAT_HELPER(formatxx::format(arg0, __VA_ARGS__), (expected))

void test_fixed()
{
	// can hold 9 characters and a NUL byte
	formatxx::fixed_writer<10> buffer;

	// should not truncate
	CHECK_FORMAT("test", buffer, "test");

	// should truncate
	buffer.clear();
	CHECK_FORMAT("test 1234", buffer, "test {0}", /*too big*/1234567890LL);
}

void test_integers()
{
	CHECK_FORMAT("123987", "{}", 123987);

	CHECK_FORMAT("0", "{}", 0);
	CHECK_FORMAT("-1", "{}", -1);
	CHECK_FORMAT("+1", "{:+}", +1);
	CHECK_FORMAT(" 1", "{: }", +1);

	CHECK_FORMAT("127", "{}", std::numeric_limits<std::int8_t>::max());
	CHECK_FORMAT("32767", "{}", std::numeric_limits<std::int16_t>::max());
	CHECK_FORMAT("2147483647", "{}", std::numeric_limits<std::int32_t>::max());
	CHECK_FORMAT("9223372036854775807", "{}", std::numeric_limits<std::int64_t>::max());

	// assumes two's complement implementation
	CHECK_FORMAT("-128", "{}", std::numeric_limits<std::int8_t>::min());
	CHECK_FORMAT("-32768", "{}", std::numeric_limits<std::int16_t>::min());
	CHECK_FORMAT("-2147483648", "{}", std::numeric_limits<std::int32_t>::min());
	CHECK_FORMAT("-9223372036854775808", "{}", std::numeric_limits<std::int64_t>::min());

	CHECK_FORMAT("0", "{:x}", 0);
	CHECK_FORMAT("0x0", "{:#x}", 0);
	CHECK_FORMAT("ff", "{:x}", 255);
	CHECK_FORMAT("0xff", "{:#x}", 255);
	CHECK_FORMAT("0x100", "{:#x}", 256);
	CHECK_FORMAT("0X11", "{:#X}", 17);
	CHECK_FORMAT("-0X11", "{:-#X}", -17);

	CHECK_FORMAT("101", "{:b}", 5);
	CHECK_FORMAT("-10", "{:b}", -2);
	CHECK_FORMAT("-0b10", "{:#b}", -2);
}

int __cdecl main()
{
	test_fixed();
	test_integers();

	std::cout << "formatxx passed " << (formatxx_tests - formatxx_failed) << " of " << formatxx_tests << " tests\n";
	return formatxx_failed == 0 ? 0 : 1;
}