cmake_minimum_required(VERSION 3.20)
project(vallocs LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
    message(STATUS "Building for Windows (MinGW)")
    add_definitions(-DPLATFORM_WINDOWS)
elseif(UNIX)
    message(STATUS "Building for Linux/Unix")
    add_definitions(-DPLATFORM_UNIX)
endif()

#include(cmake/CompilerFlags.cmake)

# Main library
add_library(allocator_lib
        src/alloc_core.cpp
        src/os_abstraction.cpp
)

if(WIN32)
    target_sources(allocator_lib PRIVATE src/win.cpp)
else()
    target_sources(allocator_lib PRIVATE src/linux.cpp)
endif()

target_include_directories(allocator_lib PUBLIC include)

add_executable(allocator_demo src/main.cpp)
target_link_libraries(allocator_demo PRIVATE allocator_lib)
