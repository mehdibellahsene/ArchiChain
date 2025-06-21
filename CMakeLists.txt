cmake_minimum_required(VERSION 3.20)
project(archimed C)

# Set C standard - use C17 for better MSVC compatibility
set(CMAKE_C_STANDARD 17)

# Main executable
add_executable(archimed main.c block.c pi.c utils.c wallet.c network.c performance.c menu.c)

# Link libraries
if(WIN32)
    target_link_libraries(archimed ws2_32)
else()
    target_link_libraries(archimed m pthread)
endif()

# Test executable (optional - only build if explicitly requested)
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    add_executable(test_archimed test.c block.c pi.c utils.c wallet.c network.c performance.c menu.c)

    # Link libraries for test
    if(WIN32)
        target_link_libraries(test_archimed ws2_32)
    else()
        target_link_libraries(test_archimed m pthread)
    endif()

    # Enable testing
    enable_testing()
    add_test(NAME archimed_tests COMMAND test_archimed)
endif()