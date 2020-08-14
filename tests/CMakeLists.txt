file(GLOB SRCS *.cpp)

ADD_EXECUTABLE(testHistogramGenerator ${SRCS})

TARGET_LINK_LIBRARIES(testHistogramGenerator
    gtest
	pthread
	png
)

add_test(NAME testHistogramGenerator
         COMMAND testHistogramGenerator)