cmake_minimum_required(VERSION 3.0)

project(formatxx)

add_library(formatxx STATIC
	include/formatxx/buffered.h
    include/formatxx/fixed.h
    include/formatxx/format.h
    include/formatxx/string.h
    include/formatxx/wide.h
	include/formatxx/_detail/parse_unsigned.h
	include/formatxx/_detail/format_impl.h
    include/formatxx/_detail/printf_impl.h
    include/formatxx/_detail/format_traits.h
    include/formatxx/_detail/write_integer.h
	source/format.cc
    source/wide.cc
)
target_include_directories(formatxx PUBLIC "include")
set_property(TARGET formatxx PROPERTY CXX_STANDARD 14)

add_executable(formatxx_tests
	source/tests.cc
)
set_property(TARGET formatxx_tests PROPERTY CXX_STANDARD 14)
target_link_libraries(formatxx_tests formatxx)
add_custom_command(
    TARGET formatxx_tests
    POST_BUILD
    COMMAND formatxx_tests
)
add_test(formatxX_tests COMMAND formatxx_tests)

