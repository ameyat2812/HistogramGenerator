file(GLOB SRCS *.cpp)
find_package(GTest REQUIRED)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(testHistogramGenerator ${SRCS})

target_link_libraries(testHistogramGenerator
    gtest
	pthread
	png
)
add_test(NAME testHistogramGenerator
         COMMAND testHistogramGenerator)