cmake_minimum_required(VERSION 3.13)

project(
    glad
)

set(CMAKE_CXX_STANDARD 20)

add_library(
    glad
	src/glad.cpp
)

target_include_directories(
    glad
	PRIVATE
	include
)