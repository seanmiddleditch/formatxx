cmake_minimum_required(VERSION 3.0)

project(formatxx)

set(FORMATXX_PUBLIC_HEADERS
	include/formatxx/buffered.h
    include/formatxx/fixed.h
    include/formatxx/format.h
    include/formatxx/string.h
    include/formatxx/wide.h
)
set(FORMATXX_PRIVATE_HEADERS
	include/formatxx/_detail/parse_unsigned.h
	include/formatxx/_detail/format_impl.h
    include/formatxx/_detail/printf_impl.h
    include/formatxx/_detail/format_traits.h
    include/formatxx/_detail/write_integer.h
)
set(FORMATXX_SOURCES
	source/format.cc
    source/wide.cc
)
set(FORMATXX_TESTS
    source/tests.cc
)

set(FORMATXX_FILES ${FORMATXX_PUBLIC_HEADERS} ${FORMATXX_PRIVATE_HEADERS} ${FORMATXX_SOURCES})

if(NOT DEFINED FORMATXX_BUILD_SHARED)
	set(FORMATXX_BUILD_SHARED ${BUILD_SHARED_LIBRARIES})
endif()

if(${FORMATXX_BUILD_SHARED})
	add_library(formatxx SHARED ${FORMATXX_FILES})
	target_compile_definitions(formatxx PRIVATE FORMATXX_EXPORT)
else()
	add_library(formatxx STATIC ${FORMATXX_FILES})
	target_compile_definitions(formatxx PUBLIC FORMATXX_STATIC)
endif()
target_include_directories(formatxx PUBLIC "include")
set_property(TARGET formatxx PROPERTY CXX_STANDARD 14)

add_executable(formatxx_tests ${FORMATXX_TESTS})
set_property(TARGET formatxx_tests PROPERTY CXX_STANDARD 14)
target_link_libraries(formatxx_tests formatxx)
add_custom_command(
    TARGET formatxx_tests
    POST_BUILD
    COMMAND formatxx_tests
)
add_test(formatxx_tests COMMAND formatxx_tests)
