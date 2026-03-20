cmake_minimum_required(VERSION 3.20)
project(vallocs LANGUAGES CXX)
cmake_policy(SET CMP0135 NEW)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (WIN32)
    message(STATUS "Building for Windows (MinGW)")
    add_definitions(-DPLATFORM_WINDOWS)
elseif (UNIX AND NOT APPLE)
    message(STATUS "Building for Linux/Unix")
    add_definitions(-DPLATFORM_LINUX)
endif ()

# Libs
#find_package(gRPC CONFIG REQUIRED)
#find_package(Protobuf CONFIG REQUIRED)
enable_testing()
add_subdirectory(src/allocs/stack)
add_subdirectory(src/allocs/bump)
add_subdirectory(src/allocs/pool)
add_subdirectory(src/platform)
add_subdirectory(tests)

# Libs link
add_library(allocators_lib INTERFACE)
target_link_libraries(allocators_lib INTERFACE platform_lib bump_lib)

add_executable(allocator_demo src/main.cpp)
target_link_libraries(allocator_demo PRIVATE allocators_lib)
