#include "formatxx/format.h"
#include "formatxx/std_string.h"
#include <doctest/doctest.h>
#include <ostream>

template <typename... Args>
static auto test_printf(char const* format, Args const& ... args) {
    char buffer[2014];
    int const len = std::snprintf(buffer, sizeof(buffer), format, args...);
    return std::string(buffer, len);
}

DOCTEST_TEST_CASE("printf") {
    using namespace formatxx;

    DOCTEST_SUBCASE("general") {
        DOCTEST_CHECK_EQ("abcd1234", printf_string("a%sd1%d4", "bc", 23));

        DOCTEST_CHECK_EQ("12 34", printf_string("%i %d", 12, 34));

        DOCTEST_CHECK_EQ("123 abc", printf_string("%2$d %1$s", "abc", 123));
        DOCTEST_CHECK_EQ("def 456", printf_string("%2% %1%", 456, "def"));

        DOCTEST_CHECK_EQ("  12", printf_string("%4i", 12));
    }

    DOCTEST_SUBCASE("floats") {
        DOCTEST_CHECK_EQ(test_printf("%7d", 1234), printf_string("%7d", 1234));
        DOCTEST_CHECK_EQ(test_printf("%-7d", 1234), printf_string("%-7d", 1234));
        DOCTEST_CHECK_EQ(test_printf("%+7d", 1234), printf_string("%+7d", 1234));
        DOCTEST_CHECK_EQ(test_printf("%+07d", 1234), printf_string("%+07d", 1234));
        DOCTEST_CHECK_EQ(test_printf("%-07d", 1234), printf_string("%-07d", 1234));
        DOCTEST_CHECK_EQ(test_printf("% 7d", 1234), printf_string("% 7d", 1234));
        DOCTEST_CHECK_EQ(test_printf("% +7d", 1234), printf_string("% +7d", 1234));
        DOCTEST_CHECK_EQ(test_printf("% +-7d;", 1234), printf_string("% +-7d;", 1234));
        DOCTEST_CHECK_EQ(test_printf("%.7d", 1234), printf_string("%.7d", 1234));
        DOCTEST_CHECK_EQ(test_printf("%-.7d;", 1234), printf_string("%-.7d;", 1234));
        DOCTEST_CHECK_EQ(test_printf("%#7x", 1234), printf_string("%#7x", 1234));
        DOCTEST_CHECK_EQ(test_printf("%#+7x", 1234), printf_string("%#+7x", 1234));
        DOCTEST_CHECK_EQ(test_printf("%#07x", 1234), printf_string("%#07x", 1234));
        DOCTEST_CHECK_EQ(test_printf("%-#07x;", 1234), printf_string("%-#07x;", 1234));
        DOCTEST_CHECK_EQ(test_printf("%+#07x", 1234), printf_string("%+#07x", 1234));

        DOCTEST_CHECK_EQ(test_printf("%7d", -1234), printf_string("%7d", -1234));
        DOCTEST_CHECK_EQ(test_printf("%-7d", -1234), printf_string("%-7d", -1234));
        DOCTEST_CHECK_EQ(test_printf("%+7d", -1234), printf_string("%+7d", -1234));
        DOCTEST_CHECK_EQ(test_printf("%+07d", -1234), printf_string("%+07d", -1234));
        DOCTEST_CHECK_EQ(test_printf("%-07d", -1234), printf_string("%-07d", -1234));
        DOCTEST_CHECK_EQ(test_printf("% 7d", -1234), printf_string("% 7d", -1234));
        DOCTEST_CHECK_EQ(test_printf("% +7d", -1234), printf_string("% +7d", -1234));
        DOCTEST_CHECK_EQ(test_printf("% +-7d;", -1234), printf_string("% +-7d;", -1234));
        DOCTEST_CHECK_EQ(test_printf("%.7d", -1234), printf_string("%.7d", -1234));
        DOCTEST_CHECK_EQ(test_printf("%-.7d;", -1234), printf_string("%-.7d;", -1234));
        DOCTEST_CHECK_EQ(test_printf("%#7x", -1234), printf_string("%#7x", -1234));
        DOCTEST_CHECK_EQ(test_printf("%#+7x", -1234), printf_string("%#+7x", -1234));
        DOCTEST_CHECK_EQ(test_printf("%#07x", -1234), printf_string("%#07x", -1234));
        DOCTEST_CHECK_EQ(test_printf("%-#07x;", -1234), printf_string("%-#07x;", -1234));
        DOCTEST_CHECK_EQ(test_printf("%+#07x", -1234), printf_string("%+#07x", -1234));
    }
}
