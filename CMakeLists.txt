cmake_minimum_required(VERSION 3.0)

project(formatxx)
enable_testing()

set(FORMATXX_PUBLIC_HEADERS
	include/formatxx/buffered.h
    include/formatxx/fixed.h
    include/formatxx/format.h
    include/formatxx/string.h
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
source_group("Header Files\\_detail" FILES ${FORMATXX_PRIVATE_HEADERS})

add_executable(formatxx_tests ${FORMATXX_TESTS})
target_link_libraries(formatxx_tests formatxx)
add_test(formatxx_tests formatxx_tests)

# variable templates are in VC 15, but Clang/GCC require -std=c++14
if(MSVC)
	set_property(TARGET formatxx PROPERTY CXX_STANDARD 11)
	set_property(TARGET formatxx_tests PROPERTY CXX_STANDARD 11)
else()
	set_property(TARGET formatxx PROPERTY CXX_STANDARD 14)
	set_property(TARGET formatxx_tests PROPERTY CXX_STANDARD 14)
endif()


# Visual Studio warns about copy_n, even though it's used correctly
# while this affects users, we don't want to indiscriminately disable
# warnings on their prorjects, so we keep this private
# FIXME - can this just be done around the offending lines?
if(MSVC)
	target_compile_definitions(formatxx PRIVATE -D_SCL_SECURE_NO_WARNINGS)
	target_compile_definitions(formatxx_tests PRIVATE -D_SCL_SECURE_NO_WARNINGS)
endif()
