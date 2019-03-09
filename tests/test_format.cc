#include "formatxx/format.h"
#include "formatxx/std_string.h"
#include "formatxx/fixed_writer.h"
#include <doctest/doctest.h>
#include <ostream>

DOCTEST_TEST_CASE("format") {
    using namespace formatxx;

    DOCTEST_SUBCASE("integers") {
        DOCTEST_CHECK_EQ("123987", format_string("{}", 123987));

        DOCTEST_CHECK_EQ("0", format_string("{}", 0));
        DOCTEST_CHECK_EQ("-1", format_string("{}", -1));
        DOCTEST_CHECK_EQ("1", format_string("{}", +1));
        DOCTEST_CHECK_EQ("+1", format_string("{:+}", +1));
        DOCTEST_CHECK_EQ(" 1", format_string("{: }", +1));

        // should all be identical - https://stackoverflow.com/questions/15333023/are-zero-padded-width-and-precision-the-same-for-integer-arguments-to-printf
        DOCTEST_CHECK_EQ("000004D2", format_string("{:08X}", 1234));
        DOCTEST_CHECK_EQ("000004D2", format_string("{:.8X}", 1234));
        DOCTEST_CHECK_EQ("000004D2", format_string("{:.08X}", 1234));

        DOCTEST_CHECK_EQ("127", format_string("{}", std::numeric_limits<std::int8_t>::max()));
        DOCTEST_CHECK_EQ("32767", format_string("{}", std::numeric_limits<std::int16_t>::max()));
        DOCTEST_CHECK_EQ("2147483647", format_string("{}", std::numeric_limits<std::int32_t>::max()));
        DOCTEST_CHECK_EQ("9223372036854775807", format_string("{}", std::numeric_limits<std::int64_t>::max()));

        // assumes two's complement implementation
        DOCTEST_CHECK_EQ("-128", format_string("{}", std::numeric_limits<std::int8_t>::min()));
        DOCTEST_CHECK_EQ("-32768", format_string("{}", std::numeric_limits<std::int16_t>::min()));
        DOCTEST_CHECK_EQ("-2147483648", format_string("{}", std::numeric_limits<std::int32_t>::min()));
        DOCTEST_CHECK_EQ("-9223372036854775808", format_string("{}", std::numeric_limits<std::int64_t>::min()));

        DOCTEST_CHECK_EQ("0", format_string("{:x}", 0));
        DOCTEST_CHECK_EQ("0x0", format_string("{:#x}", 0));
        DOCTEST_CHECK_EQ("ff", format_string("{:x}", 255));
        DOCTEST_CHECK_EQ("0xff", format_string("{:#x}", 255));
        DOCTEST_CHECK_EQ("0x100", format_string("{:#x}", 256));
        DOCTEST_CHECK_EQ("0X11", format_string("{:#X}", 17));
        DOCTEST_CHECK_EQ("0XFFFFFFEF", format_string("{:-#X}", -17));

        DOCTEST_CHECK_EQ("101", format_string("{:b}", 5));
        DOCTEST_CHECK_EQ("-10", format_string("{:b}", -2));
        DOCTEST_CHECK_EQ("-0b10", format_string("{:#b}", -2));

        DOCTEST_CHECK_EQ("11", format_string("{:o}", 9));
        DOCTEST_CHECK_EQ("-33", format_string("{:o}", -27));
        DOCTEST_CHECK_EQ("-0o10", format_string("{:#o}", -8));

        DOCTEST_CHECK_EQ("   1234", format_string("{:7d}", 1234));
        DOCTEST_CHECK_EQ("1234   ;", format_string("{:-7d};", 1234));
        DOCTEST_CHECK_EQ("0001234", format_string("{:07d}", 1234));
        DOCTEST_CHECK_EQ("1234", format_string("{:2d}", 1234));
        DOCTEST_CHECK_EQ("  +1234", format_string("{:+7d}", 1234));
        DOCTEST_CHECK_EQ("+001234", format_string("{:+07d}", 1234));
        DOCTEST_CHECK_EQ("+0001234", format_string("{:+.7d}", 1234));
    }

    DOCTEST_SUBCASE("floats") {
        DOCTEST_CHECK_EQ("123987.456000", format_string("{}", 123987.456));

        DOCTEST_CHECK_EQ("0.000000", format_string("{}", 0.0));
        DOCTEST_CHECK_EQ("1.000000", format_string("{}", 1.0));
        DOCTEST_CHECK_EQ("-1.000000", format_string("{}", -1.0));

        DOCTEST_CHECK_EQ("12.34", format_string("{:2.2}", 12.34));
        DOCTEST_CHECK_EQ("12.00", format_string("{:#2.2}", 12.0));
        DOCTEST_CHECK_EQ(" 12.34", format_string("{: 6.2}", 12.34));
        DOCTEST_CHECK_EQ("012.34", format_string("{:06.2}", 12.34));
        DOCTEST_CHECK_EQ("+12.34", format_string("{:+06.2}", 12.34));
        DOCTEST_CHECK_EQ("12.34 ;", format_string("{:-6.2};", 12.34));

        // assumes IEEE754 single- and double-precision types
        DOCTEST_CHECK_EQ("340282346638528859811704183484516925440.000000", format_string("{}", std::numeric_limits<float>::max()));
        DOCTEST_CHECK_EQ("17976931348623157081452742373170435679807056752584499659891747680315"
            "72607800285387605895586327668781715404589535143824642343213268894641827684675"
            "46703537516986049910576551282076245490090389328944075868508455133942304583236"
            "90322294816580855933212334827479782620414472316873817718091929988125040402618"
            "4124858368.000000", format_string("{}", std::numeric_limits<double>::max()));

        DOCTEST_CHECK_EQ("234987324.454500", format_string("{:f}", 234987324.4545));
        DOCTEST_CHECK_EQ("2.34987e+08", format_string("{:g}", 234987324.4545));
        DOCTEST_CHECK_EQ("0x1.c033e78e8b439p+27", format_string("{:a}", 234987324.4545));
        DOCTEST_CHECK_EQ("234987324.454500", format_string("{:F}", 234987324.4545));
        DOCTEST_CHECK_EQ("2.34987E+08", format_string("{:G}", 234987324.4545));
        DOCTEST_CHECK_EQ("0X1.C033E78E8B439P+27", format_string("{:A}", 234987324.4545));
    }

    DOCTEST_SUBCASE("strings") {
        DOCTEST_CHECK_EQ("test", format_string("{}", "test"));
        DOCTEST_CHECK_EQ("test", format_string("{}", std::string("test")));
        DOCTEST_CHECK_EQ("test", format_string("{}", formatxx::string_view("test")));

        DOCTEST_CHECK_EQ("abcdef", format_string("{}{}{}", formatxx::string_view("ab"), std::string("cd"), "ef"));

        DOCTEST_CHECK_EQ("abc", format_string(std::string("a{}c"), "b"));

        DOCTEST_CHECK_EQ("    test", format_string("{:8s}", "test"));
        DOCTEST_CHECK_EQ("test    ;", format_string("{:-8s};", "test"));

        DOCTEST_CHECK_EQ("value   00042", format_string("{:-8}{:05}", "value", 42));
    }

    DOCTEST_SUBCASE("booleans") {
        DOCTEST_CHECK_EQ("true", format_string("{}", true));
        DOCTEST_CHECK_EQ("false", format_string("{}", false));
    }

    DOCTEST_SUBCASE("pointers") {
        void const* ptr = reinterpret_cast<void const*>(static_cast<std::uintptr_t>(0xDEADC0DE));
        int const* iptr = reinterpret_cast<int const*>(static_cast<std::uintptr_t>(0xFEFEFEFE));

        DOCTEST_CHECK_EQ("DEADC0DE", format_string("{:X}", ptr));
        DOCTEST_CHECK_EQ("fefefefe", format_string("{:x}", iptr));
    }


    DOCTEST_SUBCASE("errors") {
        fixed_writer<1024> tmp;

        DOCTEST_CHECK_EQ(formatxx::result_code::success, format(tmp, "{} {:4d} {:3.5f}", "abc", 9, 12.57));
        DOCTEST_CHECK_EQ(formatxx::result_code::malformed_input, format(tmp, "{} {:4d", "abc", 9));
        DOCTEST_CHECK_EQ(formatxx::result_code::success, format(tmp, "{0} {1}", "abc", 9));
        DOCTEST_CHECK_EQ(formatxx::result_code::out_of_range, format(tmp, "{0} {1} {5}", "abc", 9, 12.57));
    }
}
