#include "formatxx/format.h"
#include "formatxx/std_string.h"
#include "formatxx/fixed_writer.h"
#include "formatxx/buffered_writer.h"
#include <doctest/doctest.h>
#include <ostream>

DOCTEST_TEST_CASE("writer") {
    using namespace formatxx;

    DOCTEST_SUBCASE("fixed") {
        // can hold 9 characters and a NUL byte
        fixed_writer<10> buffer;

        // should not truncate
        buffer.write("test");
        DOCTEST_CHECK_EQ(std::string("test"), buffer.c_str());

        // should truncate
        buffer.clear();
        format(buffer, "test {0}", /*too big*/1234567890LL);
        DOCTEST_CHECK_EQ(std::string("test 1234"), buffer.c_str());
    }

    DOCTEST_SUBCASE("string") {
        // this is mostly tested already via the CHECK_FORMAT tests everywhere else
        string_writer tmp;

        format(tmp, "1{}4", 23);
        DOCTEST_CHECK_EQ(std::string("1234"), tmp.c_str());
    }

    DOCTEST_SUBCASE("buffered") {
        buffered_writer<4> buf;

        format(buf, "1{}3", "2");
        DOCTEST_CHECK_EQ(std::string("123"), buf.c_str());

        buf.clear();
        format(buf, "1{}3{}5{}7{}9{}", 2, 4, 6, 8, 0);
        DOCTEST_CHECK_EQ(std::string("1234567890"), buf.c_str());
    }
}
