cmake_minimum_required(VERSION 3.0)

project(formatxx)

add_library(formatxx STATIC
	include/formatxx/format.h
	source/format.cc
)
target_include_directories(formatxx PUBLIC "include")
if (MSVC)
	target_compile_options(formatxx
		PUBLIC
			/std:c++latest
	)
endif()
set_property(TARGET formatxx PROPERTY CXX_STANDARD 17)
