CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(arpgstats VERSION 0.1)

add_subdirectory(external/libdicey ${CMAKE_BINARY_DIR}/libdicey)

set(ARPGSTATS_LIBS stdc++ m dicey)
enable_testing()

ADD_EXECUTABLE(stats_test tests/stats_test.cpp arpgstats.h external/libdicey/dmath.h external/libdicey/perten.h)
TARGET_INCLUDE_DIRECTORIES(stats_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
TARGET_LINK_LIBRARIES(stats_test ${ARPGSTATS_LIBS})
ADD_TEST(NAME stats_test COMMAND stats_test)

ADD_EXECUTABLE(mods_test tests/mods_test.cpp mods.cpp mods.h)
TARGET_INCLUDE_DIRECTORIES(mods_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
TARGET_LINK_LIBRARIES(mods_test ${ARPGSTATS_LIBS})
ADD_TEST(NAME mods_test COMMAND mods_test ${CMAKE_CURRENT_SOURCE_DIR}/data/test/combined.csv)

ADD_EXECUTABLE(item_test item.cpp item.h tests/item_test.cpp)
TARGET_INCLUDE_DIRECTORIES(item_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
TARGET_LINK_LIBRARIES(item_test ${ARPGSTATS_LIBS})
ADD_TEST(NAME item_test COMMAND item_test ${CMAKE_CURRENT_SOURCE_DIR}/data/test/combined.csv ${CMAKE_CURRENT_SOURCE_DIR}/data/test/currencies.csv)

ADD_EXECUTABLE(creature_test creature.cpp creature.h tests/creature_test.cpp misc.cpp misc.h)
TARGET_INCLUDE_DIRECTORIES(creature_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
TARGET_LINK_LIBRARIES(creature_test ${ARPGSTATS_LIBS})
ADD_TEST(NAME creature_test COMMAND creature_test ${CMAKE_CURRENT_SOURCE_DIR}/data/test/monsters.csv)
