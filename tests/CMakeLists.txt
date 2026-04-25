include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

function(add_gtest_suite NAME)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs SOURCES LIBS)
    cmake_parse_arguments(SUITE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT SUITE_SOURCES)
        message(FATAL_ERROR "add_gtest_suite(${NAME}) requires SOURCES <files...>")
    endif ()

    add_executable(${NAME} ${SUITE_SOURCES})
    target_link_libraries(${NAME}
            PRIVATE
            allocators_lib
            GTest::gtest_main
            ${SUITE_LIBS}
    )
    set_target_properties(${NAME} PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON
    )

    include(GoogleTest)
    gtest_discover_tests(${NAME}
            TEST_PREFIX "${NAME}."
            PROPERTIES LABELS "${NAME}"
            DISCOVERY_TIMEOUT 30
    )
endfunction()

# Arena allocator test suite
add_gtest_suite(bump_tests
        SOURCES
        u_test_bump.cpp
)

# Stack allocator test suite
add_gtest_suite(stack_tests
        SOURCES
        u_test_stack.cpp
)

# Pool allocator test suite
add_gtest_suite(pool_tests
        SOURCES
        u_test_pool.cpp
)

# Free list allocator test suite
add_gtest_suite(fl_tests
        SOURCES
        u_test_fl.cpp
)

# Buddy allocator test suite
add_gtest_suite(buddy_tests
        SOURCES
        u_test_buddy.cpp
)
