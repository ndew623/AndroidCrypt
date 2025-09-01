cmake_minimum_required(VERSION 3.21)

# Define the Cryptographic Hashing project
project(libhash
        VERSION 1.0.8.0
        DESCRIPTION "Cryptographic Hashing Library"
        LANGUAGES CXX)

# Set options depending on whether this is a subproject
if(PROJECT_IS_TOP_LEVEL)
    # Option to control whether tests are built
    option(libhash_BUILD_TESTS "Build Tests for Hashing Library" ON)
else()
    # Option to control whether tests are built
    option(libhash_BUILD_TESTS "Build Tests for Hashing Library" OFF)
endif()

# Option to control ability to install the library
option(libhash_INSTALL "Install the Hashing Library" ON)

# Determine whether clang-tidy will be performed
option(libhash_CLANG_TIDY "Use clang-tidy to perform linting during build" OFF)

add_subdirectory(dependencies)
add_subdirectory(src)

include(CTest)

if(BUILD_TESTING AND libhash_BUILD_TESTS)
    add_subdirectory(test)
endif()
