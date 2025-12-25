CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(arpgstats VERSION 0.1)

set(ARPGSTATS_LIBS stdc++ m)
enable_testing()

ADD_EXECUTABLE(stats_test tests/stats_test.cpp arpgstats.h external/libdicey/dmath.h external/libdicey/perten.h)
TARGET_INCLUDE_DIRECTORIES(stats_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
TARGET_LINK_LIBRARIES(stats_test ${ARPGSTATS_LIBS})

ADD_TEST(stats_test stats_test)
