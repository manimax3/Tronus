cmake_minimum_required (VERSION 3.4)
project(Tronus)

option(TRONUS_BUILD_EDITOR "Build Editor" OFF)
option(TRONUS_BUILD_EXAMPLES "Build Examples" ON)
option(TRONUS_BUILD_TESTS "Build Tests" ON)


add_subdirectory(External)
add_subdirectory(Engine)

if (TRONUS_BUILD_EXAMPLES)
add_subdirectory(Editor)
endif()

if (TRONUS_BUILD_EXAMPLES)
add_subdirectory(Examples)
endif()

if (TRONUS_BUILD_TESTS)
enable_testing()
add_subdirectory(Tests)
endif()
