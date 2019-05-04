include(FetchContent)
FetchContent_Declare(doctest
    GIT_REPOSITORY https://github.com/onqtam/doctest.git
    GIT_TAG 2.3.1
    GIT_SHALLOW ON
    GIT_SUBMODULES ""
)
FetchContent_Declare(litexx
    GIT_REPOSITORY https://github.com/seanmiddleditch/litexx.git
    GIT_TAG 0.1.0
    GIT_SHALLOW ON
    GIT_SUBMODULES ""
)

if(FORMATXX_BUILD_TESTS AND NOT TARGET doctest)
    FetchContent_Populate(doctest)
    set(DOCTEST_WITH_TESTS OFF CACHE BOOL "enable doctest tests")
    set(DOCTEST_WITH_MAIN_IN_STATIC_LIB OFF CACHE BOOL "enable doctest static library")
    add_subdirectory(${doctest_SOURCE_DIR} ${doctest_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

if(NOT TARGET litexx)
    FetchContent_Populate(litexx)
    add_subdirectory(${litexx_SOURCE_DIR} ${litexx_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
