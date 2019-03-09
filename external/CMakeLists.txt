add_subdirectory(litexx EXCLUDE_FROM_ALL)

if(FORMATXX_BUILD_TESTS)
    set(DOCTEST_WITH_TESTS OFF CACHE BOOL "enable doctest tests")
    add_subdirectory(doctest EXCLUDE_FROM_ALL)
endif()
