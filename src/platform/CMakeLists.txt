#file(GLOB PLATFORM_COMMON CONFIGURE_DEPENDS
#    ${CMAKE_CURRENT_SOURCE_DIR}/platform_mem.cpp
#)

set(LIB_NAME platform_lib)

if (WIN32)
    set(PLATFORM_OS ${CMAKE_CURRENT_SOURCE_DIR}/platform_win.cpp)
    message(STATUS "platform_lib: Windows source selected")
elseif (UNIX AND NOT APPLE)
    set(PLATFORM_OS ${CMAKE_CURRENT_SOURCE_DIR}/platform_linux.cpp)
    message(STATUS "platform_lib: Linux source selected")
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

add_library(${LIB_NAME} STATIC
    ${PLATFORM_COMMON}
    ${PLATFORM_OS}
)

target_include_directories(${LIB_NAME}
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

if (WIN32)
    target_compile_definitions(${LIB_NAME} PUBLIC PLATFORM_WINDOWS)
elseif (UNIX AND NOT APPLE)
    target_compile_definitions(${LIB_NAME} PUBLIC PLATFORM_LINUX)
endif()

target_compile_features(${LIB_NAME} PUBLIC cxx_std_20)
