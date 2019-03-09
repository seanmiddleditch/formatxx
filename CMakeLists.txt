cmake_minimum_required(VERSION 3.8)

project(formatxx
	VERSION 0.9.0
	LANGUAGES CXX
)

option(FORMATXX_BUILD_TESTS "Build formatxx tests" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

add_subdirectory(external)

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
    tests/main.cc
    tests/test_format.cc
    tests/test_printf.cc
    tests/test_wide.cc
    tests/test_writer.cc
)

set(FORMATXX_FILES ${FORMATXX_PUBLIC_HEADERS} ${FORMATXX_PRIVATE_HEADERS} ${FORMATXX_SOURCES})

add_library(formatxx ${FORMATXX_FILES})
target_compile_features(formatxx PUBLIC cxx_std_17)
target_include_directories(formatxx PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include/formatxx>
)
target_link_libraries(formatxx PUBLIC litexx)
set_target_properties(formatxx PROPERTIES
    DEFINE_SYMBOL FORMATXX_EXPORT
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)
source_group("Header Files\\_detail" FILES ${FORMATXX_PRIVATE_HEADERS})

install(TARGETS formatxx EXPORT formatxx-targets
    ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${CMAKE_INSTALL_PREFIX})
install(EXPORT formatxx-targets DESTINATION share/formatxx/cmake)

export(TARGETS formatxx FILE formatxx-exports.cmake)

if(FORMATXX_BUILD_TESTS)
	enable_testing()
	add_executable(formatxx_tests ${FORMATXX_TESTS})
	target_link_libraries(formatxx_tests formatxx doctest)
	add_test(formatxx_tests formatxx_tests)
endif()
