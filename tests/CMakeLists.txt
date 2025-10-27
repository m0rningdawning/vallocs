include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

# Prevent GoogleTest from adding install rules
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(allocator_tests
        u_tests.cpp
)

target_link_libraries(allocator_tests
        PRIVATE
        allocator_lib
        GTest::gtest_main
)

set_target_properties(allocator_tests PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)

include(GoogleTest)
gtest_discover_tests(allocator_tests)
