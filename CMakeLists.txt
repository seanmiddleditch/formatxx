cmake_minimum_required(VERSION 3.8)

project(formatxx
	VERSION 0.9.0
	LANGUAGES CXX
)

option(FORMATXX_BUILD_TESTS "Build formatxx tests" ON)

set(FORMATXX_PUBLIC_HEADERS
	include/formatxx/buffered_writer.h
    include/formatxx/fixed_writer.h
    include/formatxx/format.h
    include/formatxx/std_string.h
    include/formatxx/wide.h
)
set(FORMATXX_PRIVATE_HEADERS
	include/formatxx/_detail/parse_format.h
	include/formatxx/_detail/parse_unsigned.h
	include/formatxx/_detail/format_impl.h
    include/formatxx/_detail/printf_impl.h
	include/formatxx/_detail/format_traits.h
	include/formatxx/_detail/format_util.h
	include/formatxx/_detail/write_integer.h
	include/formatxx/_detail/write_float.h
	include/formatxx/_detail/write_string.h
)
set(FORMATXX_SOURCES
	source/format.cc
    source/wide.cc
)
set(FORMATXX_TESTS
    source/tests.cc
)

set(FORMATXX_FILES ${FORMATXX_PUBLIC_HEADERS} ${FORMATXX_PRIVATE_HEADERS} ${FORMATXX_SOURCES})

add_library(formatxx ${FORMATXX_FILES})
target_compile_features(formatxx PUBLIC cxx_std_17)
target_include_directories(formatxx PUBLIC "include")
set_target_properties(formatxx PROPERTIES
    DEFINE_SYMBOL FORMATXX_EXPORT
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)
source_group("Header Files\\_detail" FILES ${FORMATXX_PRIVATE_HEADERS})

if(FORMATXX_BUILD_TESTS)
	enable_testing()
	add_executable(formatxx_tests ${FORMATXX_TESTS})
	target_link_libraries(formatxx_tests formatxx)
	add_test(formatxx_tests formatxx_tests)
endif()