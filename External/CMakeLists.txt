### Manages available dependencies

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(glfw)

# GLAD
add_library(Glad STATIC "glad/src/glad.c")
target_include_directories(Glad PUBLIC "glad")

# easy_profiler
add_subdirectory(easy_profiler)
