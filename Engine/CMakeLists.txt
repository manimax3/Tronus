set(TRONUS_SOURCE_FILES
        tr/math/Vec3.h
        tr/math/Vec3.cpp
        tr/math/Vec4.h
        tr/math/Vec4.cpp
        tr/math/Mat4.h
        tr/math/Mat4.cpp
        tr/math/Quaternion.h
        tr/math/Quaternion.cpp
        tr/math/Vec2.h
        tr/math/Math.h
        tr/core/JobHandler.h
        tr/core/JobHandler.cpp tr/core/Subsystem.h tr/core/Subsystem.cpp tr/core/Engine.h tr/core/Engine.cpp tr/util/Timer.h tr/util/Timer.cpp tr/profile/Profiler.h tr/profile/Profiler.cpp tr/filesystem/StreamBuffer.cpp tr/util/Log.h tr/util/Log.cpp tr/event/EventSystem.h tr/event/EventSystem.cpp tr/graphics/GraphicsHandler.h tr/graphics/GraphicsHandler.cpp)

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

target_link_libraries(Tronus easy_profiler)

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
