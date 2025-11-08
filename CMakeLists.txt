cmake_minimum_required(VERSION 3.20)
project(vallocs LANGUAGES CXX)
cmake_policy(SET CMP0135 NEW)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (WIN32)
    message(STATUS "Building for Windows (MinGW)")
    add_definitions(-DPLATFORM_WINDOWS)
elseif (UNIX)
    message(STATUS "Building for Linux/Unix")
    add_definitions(-DPLATFORM_UNIX)
endif ()

#include(cmake/CompilerFlags.cmake)

# Allocs
add_subdirectory(src/arena)

# Libs link
add_library(allocators_lib INTERFACE)
target_link_libraries(allocators_lib INTERFACE arena_lib)

#if(WIN32)
#    target_sources(allocator_lib PRIVATE src/win.cpp)
#else()
#    target_sources(allocator_lib PRIVATE src/linux.cpp)
#endif()

add_executable(allocator_demo src/main.cpp)
target_link_libraries(allocator_demo PRIVATE allocators_lib)

enable_testing()
add_subdirectory(tests)