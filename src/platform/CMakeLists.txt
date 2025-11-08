file(GLOB PLATFORM_COMMON CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/platform_mem.cpp
)

set(LIB_NAME platform_lib)

if(WIN32)
    set(PLATFORM_OS ${CMAKE_CURRENT_SOURCE_DIR}/platform_win.cpp)
elseif(LINUX)
    set(PLATFORM_OS ${CMAKE_CURRENT_SOURCE_DIR}/platform_unix.cpp)
endif()

add_library(${LIB_NAME} STATIC
        ${PLATFORM_COMMON}
        ${PLATFORM_OS}
)

target_include_directories(platform_lib
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(platform_lib PUBLIC cxx_std_20)

target_compile_definitions(platform_lib
        PUBLIC
        $<$<BOOL:WIN32>:PLATFORM_WINDOWS>
        $<$<BOOL:LINUX>:PLATFORM_LINUX>
)
