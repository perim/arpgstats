CMAKE_MINIMUM_REQUIRED(VERSION 3.22)
PROJECT(arpgstats VERSION 0.1)

add_subdirectory(external/libdicey ${CMAKE_BINARY_DIR}/libdicey)

# Copy the test data into the build directory. This is a build rule with the data files as
# dependencies, so the copy stays up to date when they change (no reconfigure needed).
FILE(GLOB_RECURSE ARPGSTATS_DATA_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/data/*")
ADD_CUSTOM_COMMAND(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/data.stamp
	 COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_CURRENT_BINARY_DIR}/data
	 COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data
	 COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/data.stamp
	 DEPENDS ${ARPGSTATS_DATA_FILES}
	 COMMENT "Copying test data")
ADD_CUSTOM_TARGET(data_copy ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/data.stamp)

set(ARPGSTATS_LIBS stdc++ m dicey)
enable_testing()
set(CMAKE_CTEST_ARGUMENTS "--output-on-failure")

ADD_EXECUTABLE(stats_test tests/stats_test.cpp arpgstats.h external/libdicey/dmath.h external/libdicey/perten.h)
TARGET_INCLUDE_DIRECTORIES(stats_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
TARGET_LINK_LIBRARIES(stats_test ${ARPGSTATS_LIBS})
ADD_TEST(NAME stats_test COMMAND stats_test)

ADD_EXECUTABLE(mods_test tests/mods_test.cpp mods.cpp mods.h)
TARGET_INCLUDE_DIRECTORIES(mods_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
TARGET_LINK_LIBRARIES(mods_test ${ARPGSTATS_LIBS})
ADD_DEPENDENCIES(mods_test data_copy)
ADD_TEST(NAME mods_test COMMAND mods_test)

ADD_EXECUTABLE(item_test item.cpp item.h mods.cpp mods.h tests/item_test.cpp)
TARGET_INCLUDE_DIRECTORIES(item_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
TARGET_LINK_LIBRARIES(item_test ${ARPGSTATS_LIBS})
ADD_DEPENDENCIES(item_test data_copy)
ADD_TEST(NAME item_test COMMAND item_test)

ADD_EXECUTABLE(creature_test creature.cpp creature.h tests/creature_test.cpp misc.cpp misc.h)
TARGET_INCLUDE_DIRECTORIES(creature_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
TARGET_LINK_LIBRARIES(creature_test ${ARPGSTATS_LIBS})
ADD_DEPENDENCIES(creature_test data_copy)
ADD_TEST(NAME creature_test COMMAND creature_test)

ADD_EXECUTABLE(csv_test tests/csv_test.cpp csv.h)
TARGET_INCLUDE_DIRECTORIES(csv_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
TARGET_LINK_LIBRARIES(csv_test ${ARPGSTATS_LIBS})
ADD_TEST(NAME csv_test COMMAND csv_test)
