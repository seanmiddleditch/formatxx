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
#include <formatxx/fixed.h>
#include <formatxx/buffered.h>
#include <formatxx/wide.h>
#include <formatxx/string.h>

#include <iostream>
#include <string>
#include <cstdint>
#include <limits>

static int formatxx_tests = 0;
static int formatxx_failed = 0;

static auto formatxx_tostring(std::string&& str) { return std::move(str); }
static auto formatxx_tostring(std::wstring&& str) { return std::move(str); }
template <typename CharT> static std::basic_string<CharT, std::char_traits<CharT>, std::allocator<CharT>> formatxx_tostring(formatxx::basic_format_writer<CharT> const& writer) { return {writer.view().data(), writer.view().size()}; }

template <typename CharT, typename T>
static std::basic_string<CharT> format_value_to_string(T const& value, formatxx::basic_string_view<CharT> spec)
{
	using formatxx::format_value;
	formatxx::basic_string_writer<std::basic_string<CharT>> buf;
	format_value(buf, value, spec);
	return std::move(buf).str();
}

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

#define CHECK_WFORMAT_HELPER(expr, expected) \
	do{ \
		++formatxx_tests; \
		auto const& result = formatxx_tostring((expr)); \
		if (result == (expected)) {} else { \
			std::wcerr << __FILE__ << L'(' << __LINE__ << L"): TEST FAILED\n"; \
			std::wcerr << #expr << L'\n'; \
			std::wcerr << L"  Expected: " << (expected) << L'\n'; \
			std::wcerr << L"  Received: " << result << L'\n'; \
			++formatxx_failed; \
		} \
	}while(false)

#define CHECK_FORMAT_CALL(expected, formatter, arg0, ...) \
	CHECK_FORMAT_HELPER(formatter(arg0, __VA_ARGS__), (expected))

#define CHECK_WFORMAT_CALL(expected, formatter, arg0, ...) \
	CHECK_WFORMAT_HELPER(formatter(arg0, __VA_ARGS__), (expected))

#define CHECK_FORMAT(expected, arg0, ...) \
	CHECK_FORMAT_CALL((expected), formatxx::sformat, arg0, __VA_ARGS__)

#define CHECK_WFORMAT(expected, arg0, ...) \
	CHECK_WFORMAT_CALL((expected), formatxx::sformat<std::wstring>, arg0, __VA_ARGS__)

#define CHECK_PRINTF(expected, arg0, ...) \
	CHECK_FORMAT_CALL((expected), formatxx::sprintf, arg0, __VA_ARGS__)

#define CHECK_FORMAT_VALUE(expected, value, spec) \
	CHECK_FORMAT_CALL((expected), format_value_to_string<char>, (value), (spec))

#define CHECK_WFORMAT_VALUE(expected, value, spec) \
	CHECK_WFORMAT_CALL((expected), format_value_to_string<wchar_t>, (value), (spec))

#define CHECK_FORMAT_WRITER(expected, arg0, ...) \
	CHECK_FORMAT_CALL((expected), formatxx::format, arg0, __VA_ARGS__)

static void test_fixed()
{
	// can hold 9 characters and a NUL byte
	formatxx::fixed_writer<10> buffer;

	// should not truncate
	CHECK_FORMAT_WRITER("test", buffer, "test");

	// should truncate
	buffer.clear();
	CHECK_FORMAT_WRITER("test 1234", buffer, "test {0}", /*too big*/1234567890LL);
}

static void test_integers()
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

// FIXME: currently platform-dependent due to sprintf dependence
static void test_floats()
{
	CHECK_FORMAT("123987.456000", "{}", 123987.456);

	CHECK_FORMAT("0.000000", "{}", 0.0);
	CHECK_FORMAT("1.000000", "{}", 1.0);
	CHECK_FORMAT("-1.000000", "{}", -1.0);

	CHECK_FORMAT("340282346638528859811704183484516925440.000000", "{}", std::numeric_limits<float>::max());
	CHECK_FORMAT("17976931348623157081452742373170435679807056752584499659891747680315"
		"72607800285387605895586327668781715404589535143824642343213268894641827684675"
		"46703537516986049910576551282076245490090389328944075868508455133942304583236"
		"90322294816580855933212334827479782620414472316873817718091929988125040402618"
		"4124858368.000000", "{}", std::numeric_limits<double>::max());

	CHECK_FORMAT("234987324.454500", "{:f}", 234987324.4545);
	CHECK_FORMAT("2.34987e+08", "{:g}", 234987324.4545);
	CHECK_FORMAT("0x1.c033e78e8b439p+27", "{:a}", 234987324.4545);
	CHECK_FORMAT("234987324.454500", "{:F}", 234987324.4545);
	CHECK_FORMAT("2.34987E+08", "{:G}", 234987324.4545);
	CHECK_FORMAT("0X1.C033E78E8B439P+27", "{:A}", 234987324.4545);
}

// this is mostly tested already via the CHECK_FORMAT tests everywhere else
static void test_string_writer()
{
	formatxx::string_writer tmp;

	CHECK_FORMAT_WRITER("1234", tmp, "1{}4", 23);
}

static void test_buffered()
{
	formatxx::buffered_writer<4> buf;

	CHECK_FORMAT_WRITER("123", buf, "1{}3", "2");

	buf.clear();

	CHECK_FORMAT_WRITER("1234567890", buf, "1{}3{}5{}7{}9{}", 2, 4, 6, 8, 0);
}

static void test_printf()
{
	CHECK_PRINTF("abcd1234", "a%sd1%d4", "bc", 23);
}

static void test_strings()
{
	CHECK_FORMAT_VALUE("test", "test", "");
	CHECK_FORMAT_VALUE("test", std::string("test"), "");
	CHECK_FORMAT_VALUE("test", formatxx::string_view("test"), "");

	CHECK_FORMAT("abcdef", "{}{}{}", formatxx::string_view("ab"), std::string("cd"), "ef");

	CHECK_FORMAT("abc", std::string("a{}c"), "b");
}

static void test_wide_strings()
{
	CHECK_WFORMAT_VALUE(L"1234", 1234U, L"");
	CHECK_WFORMAT_VALUE(L"-47", -47, L"");

	CHECK_WFORMAT(L"abcd1234", L"{}{}{}{}", L"ab", L'c', L'd', L"1234");
}

static void test_bool()
{
	CHECK_FORMAT("true", "{}", true);
	CHECK_FORMAT("false", "{}", false);
}

static void test_pointers()
{
	void const* ptr = reinterpret_cast<void const*>(static_cast<std::uintptr_t>(0xDEADC0DE));
	int const* iptr = reinterpret_cast<int const*>(static_cast<std::uintptr_t>(0xFEFEFEFE));

	CHECK_FORMAT("DEADC0DE", "{:X}", ptr);
	CHECK_FORMAT("fefefefe", "{:x}", iptr);
}

#if defined(WIN32)
// sometimes useful to compile a whole project with /Gv or the like
// but that breaks test files
#	define FORMATXX_MAIN_DECL __cdecl
#else
#	define FORMATXX_MAIN_DECL
#endif

int FORMATXX_MAIN_DECL main()
{
	test_fixed();
	test_integers();
	test_floats();
	test_string_writer();
	test_buffered();
	test_printf();
	test_strings();
	test_wide_strings();
	test_bool();
	test_pointers();

	std::cout << "formatxx passed " << (formatxx_tests - formatxx_failed) << " of " << formatxx_tests << " tests\n";
	return formatxx_failed == 0 ? 0 : 1;
}