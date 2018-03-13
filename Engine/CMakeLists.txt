FUNCTION(PREPEND var prefix)
   SET(listVar "")
   FOREACH(f ${ARGN})
      LIST(APPEND listVar "${prefix}/${f}")
   ENDFOREACH(f)
   SET(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(PREPEND)

set(TRONUS_CORE_SRC JobHandler.cpp Subsystem.cpp Engine.cpp)
set(TRONUS_MATH_SRC Vec3.cpp Vec4.cpp Mat4.cpp Quaternion.cpp)
set(TRONUS_PROFILE_SRC Profiler.cpp)
set(TRONUS_UTIL_SRC Timer.cpp Log.cpp stb_impl.cpp)
set(TRONUS_EVENT_SRC EventSystem.cpp)
set(TRONUS_FILESYSTEM_SRC StreamBuffer.cpp ResourceManager.cpp Filesystem.cpp)
set(TRONUS_GRAPHICS_SRC GraphicsHandler.cpp GLCheck.cpp GLSLShader.cpp Simple2DRenderer.cpp Renderer2D.cpp Image.cpp Texture.cpp ImguiRenderer.cpp)

PREPEND(TRONUS_CORE_SRC tr/core ${TRONUS_CORE_SRC})
PREPEND(TRONUS_MATH_SRC tr/math ${TRONUS_MATH_SRC})
PREPEND(TRONUS_UTIL_SRC tr/util ${TRONUS_UTIL_SRC})
PREPEND(TRONUS_EVENT_SRC tr/event ${TRONUS_EVENT_SRC})
PREPEND(TRONUS_PROFILE_SRC tr/profile ${TRONUS_PROFILE_SRC})
PREPEND(TRONUS_FILESYSTEM_SRC tr/filesystem ${TRONUS_FILESYSTEM_SRC})
PREPEND(TRONUS_GRAPHICS_SRC tr/graphics ${TRONUS_GRAPHICS_SRC})

set(TRONUS_SOURCE_FILES ${TRONUS_CORE_SRC} ${TRONUS_MATH_SRC} ${TRONUS_UTIL_SRC} ${TRONUS_EVENT_SRC} ${TRONUS_PROFILE_SRC} ${TRONUS_FILESYSTEM_SRC} ${TRONUS_GRAPHICS_SRC})

option(TRONUS_BUILD_SHARED off)

if(TRONUS_BUILD_SHARED)
  add_library(Tronus SHARED ${TRONUS_SOURCE_FILES})
else()
  add_library(Tronus STATIC ${TRONUS_SOURCE_FILES})
endif()

target_include_directories(Tronus PUBLIC tr)
target_include_directories(Tronus INTERFACE .)

## Dependencies: Link and include

target_link_libraries(Tronus glfw)
target_include_directories(Tronus PUBLIC "${CMAKE_SOURCE_DIR}/External/glfw/include")

target_include_directories(Tronus PUBLIC "${CMAKE_SOURCE_DIR}/External/concurrentqueue")
target_include_directories(Tronus PRIVATE "${CMAKE_SOURCE_DIR}/External/rang/include")
target_include_directories(Tronus PRIVATE "${CMAKE_SOURCE_DIR}/External/json/include")
target_include_directories(Tronus PRIVATE "${CMAKE_SOURCE_DIR}/External/stb")

target_link_libraries(Tronus easy_profiler)
target_link_libraries(Tronus Glad)
target_link_libraries(Tronus ImGui)

## Compile options

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_compile_options(Tronus PUBLIC -std=c++17)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  target_compile_options(Tronus PUBLIC -std=c++17)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  # using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  target_compile_options(Tronus PUBLIC /std:c++latest /MP)
endif()

install(TARGETS Tronus
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)
