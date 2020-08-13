# CMakeList.txt : Top-level CMake project file
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    FORCE)

project ("HistogramGenerator")

add_executable (HistogramGenerator "HistogramGenerator.cpp" "Image.h")

target_link_libraries(HistogramGenerator png )

