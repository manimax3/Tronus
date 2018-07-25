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

#imgui
add_library(ImGui STATIC "imgui/imgui.cpp" "imgui/imgui_demo.cpp" "imgui/imgui_draw.cpp")
target_include_directories(ImGui INTERFACE imgui)

#Box2D
file(GLOB box2d_source_files Box2D/Box2D/**/*.cpp)
add_library(Box2D STATIC ${box2d_source_files})
target_include_directories(Box2D PUBLIC Box2D)
