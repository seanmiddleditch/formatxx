add_subdirectory(litexx EXCLUDE_FROM_ALL)

if(FORMATXX_BUILD_TESTS AND NOT TARGET doctest)
    set(DOCTEST_WITH_TESTS OFF CACHE BOOL "enable doctest tests")
    set(DOCTEST_WITH_MAIN_IN_STATIC_LIB OFF CACHE BOOL "enable doctest static library")
    add_subdirectory(doctest EXCLUDE_FROM_ALL)
endif()
