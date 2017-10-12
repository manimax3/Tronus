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
        tr/math/Math.h)

option(TRONUS_BUILD_SHARED off)

if(TRONUS_BUILD_SHARED)
  add_library(Tronus SHARED ${TRONUS_SOURCE_FILES})
else()
  add_library(Tronus STATIC ${TRONUS_SOURCE_FILES})
endif()

target_include_directories(Tronus PRIVATE tr)

## Dependencies: Link and include

target_link_libraries(Tronus glfw)
target_include_directories(Tronus PUBLIC "${CMAKE_SOURCE_DIR}/External/glfw/include")

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
