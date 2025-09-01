cmake_minimum_required(VERSION 3.21)

# Define the Console I/O Library Project
project(conio
        VERSION 1.0.4.0
        DESCRIPTION "Console I/O Library"
        LANGUAGES CXX)

# Set options depending on whether this is a subproject
if(PROJECT_IS_TOP_LEVEL)
    # Option to control whether tests are built
    option(conio_BUILD_TESTS "Build Tests for the Console I/O Library" ON)
else()
    # Option to control whether tests are built
    option(conio_BUILD_TESTS "Build Tests for the Console I/O Library" OFF)
endif()

# Option to control ability to install the library
option(conio_INSTALL "Install the Console I/O Library" ON)

# Determine whether clang-tidy will be performed
option(conio_CLANG_TIDY "Use clang-tidy to perform linting during build" OFF)

add_subdirectory(dependencies)
add_subdirectory(src)

include(CTest)

if(BUILD_TESTING AND conio_BUILD_TESTS)
    add_subdirectory(ansi)
    add_subdirectory(show_progress)
    add_subdirectory(test)
endif()
