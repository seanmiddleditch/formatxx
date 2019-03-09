#include "formatxx/format.h"
#include "formatxx/std_string.h"
#include "formatxx/wide.h"
#include <doctest/doctest.h>
#include <ostream>

DOCTEST_TEST_CASE("wide") {
    using namespace formatxx;

    DOCTEST_SUBCASE("format") {
        DOCTEST_CHECK_EQ(L"1234", format_string<std::wstring>(L"{}", 1234U));
        DOCTEST_CHECK_EQ(L"-17.500000", format_string<std::wstring>(L"{}", -17.5));
        DOCTEST_CHECK_EQ(L"true", format_string<std::wstring>(L"{}", true));
        DOCTEST_CHECK_EQ(L"lorem ipsum", format_string<std::wstring>(L"{}", "lorem ipsum"));
        DOCTEST_CHECK_EQ("lorem ipsum", format_string("{}", L"lorem ipsum"));
        DOCTEST_CHECK_EQ(L"lorem ipsum", format_string<std::wstring>(L"{}", std::string("lorem ipsum")));
        DOCTEST_CHECK_EQ("lorem ipsum", format_string("{}", std::wstring(L"lorem ipsum")));

        DOCTEST_CHECK_EQ(L"abcd1234", format_string<std::wstring>(L"{}{}{}{}{}", L"ab", L'c', L'd', 12, 34UL));
        DOCTEST_CHECK_EQ(L"the lazy fox", format_string<std::wstring>(L"{} {} {}{}{}", L"the", "lazy", L'f', 'o', L'x'));
        DOCTEST_CHECK_EQ("this is a test", format_string("this {} a {}{}{}", L"is", 't', L'e', L"st"));
    }

    DOCTEST_SUBCASE("printf") {
        DOCTEST_CHECK_EQ(L"12abcd34", printf_string<std::wstring>(L"%d%s%c%c%d", 12, L"ab", 'c', L'd', 34UL));
    }
}
