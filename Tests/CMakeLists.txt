set(TEST_SOURCES main.cpp jobhandler_test.cpp engine_test.cpp filesystem_test.cpp log_test.cpp gameobject_test.cpp)

add_executable(Tests "${TEST_SOURCES}")
target_include_directories(Tests PRIVATE "${CMAKE_SOURCE_DIR}/External/Catch/single_include")
target_link_libraries(Tests Tronus)

add_test(test Tests)
