#include "formatxx/small_string.h"
#include <doctest/doctest.h>

DOCTEST_TEST_CASE("small_string") {
    using namespace formatxx;

    DOCTEST_SUBCASE("initialization") {
        small_string<char, 512> buffer;

        DOCTEST_CHECK(buffer.empty());
        DOCTEST_CHECK(!buffer);
        DOCTEST_CHECK_EQ(512, buffer.capacity());
        DOCTEST_CHECK_EQ(0, buffer.size());
        DOCTEST_CHECK_EQ(0, std::strcmp("", buffer.c_str()));
    }

    DOCTEST_SUBCASE("append") {
        small_string<char, 512> buffer;

        buffer.append("test", 4);
        buffer.append(" ", 1);
        buffer.append("string", 6);

        DOCTEST_CHECK(!buffer.empty());
        DOCTEST_CHECK(buffer);
        DOCTEST_CHECK_EQ(512, buffer.capacity());
        DOCTEST_CHECK_EQ(11, buffer.size());
        DOCTEST_CHECK_EQ(0, std::strcmp("test string", buffer.c_str()));
    }

    DOCTEST_SUBCASE("overflow") {
        small_string<char, 5> buffer;

        buffer.append("abc", 3);
        buffer.append("def", 3);
        buffer.append("ghi", 3);
        buffer.append("jkl", 3);
        buffer.append("mno", 3);

        DOCTEST_CHECK(!buffer.empty());
        DOCTEST_CHECK(buffer);
        DOCTEST_CHECK_LE(15, buffer.capacity());
        DOCTEST_CHECK_EQ(15, buffer.size());
        DOCTEST_CHECK_EQ(0, std::strcmp("abcdefghijklmno", buffer.c_str()));
    }

    DOCTEST_SUBCASE("clear") {
        small_string<char, 5> buffer;

        buffer.append("abc", 3);
        buffer.clear();

        DOCTEST_CHECK(buffer.empty());
        DOCTEST_CHECK(!buffer);
        DOCTEST_CHECK_EQ(5, buffer.capacity());
        DOCTEST_CHECK_EQ(0, buffer.size());
        DOCTEST_CHECK_EQ(0, std::strcmp("", buffer.c_str()));

        buffer.append("abcdef", 6);
        buffer.clear();

        DOCTEST_CHECK(buffer.empty());
        DOCTEST_CHECK(!buffer);
        DOCTEST_CHECK_LT(5, buffer.capacity());
        DOCTEST_CHECK_EQ(0, buffer.size());
        DOCTEST_CHECK_EQ(0, std::strcmp("", buffer.c_str()));
    }
}
