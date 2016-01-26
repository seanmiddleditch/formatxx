cmake_minimum_required(VERSION 3.0)

project(formatxx)

add_library(formatxx STATIC
	include/formatxx/format.h
	include/formatxx/stdfmt.h
	source/format.cc
)
target_include_directories(formatxx PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
set_property(TARGET formatxx PROPERTY CXX_STANDARD 14)
