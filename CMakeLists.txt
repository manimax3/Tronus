cmake_minimum_required (VERSION 3.4)
project(Tronus)

option(TRONUS_BUILD_EDITOR on)
option(TRONUS_BUILD_EXAMPLES on)
option(TRONUS_BUILD_TESTS on)


add_subdirectory(External)
add_subdirectory(Engine)
add_subdirectory(Editor)
add_subdirectory(Examples)

enable_testing()
add_subdirectory(Tests)
