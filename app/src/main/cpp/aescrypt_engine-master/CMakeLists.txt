cmake_minimum_required(VERSION 3.21)

# Define the AES Crypt Engine project
project(aescrypt_engine
        VERSION 4.1.1.0
        DESCRIPTION "AES Crypt Engine"
        LANGUAGES CXX)

# Tests are built by default when this is a top-level project
if(PROJECT_IS_TOP_LEVEL)
    option(aescrypt_engine_BUILD_TESTS "Build Tests for the AES Crypt Engine" ON)
else()
    option(aescrypt_engine_BUILD_TESTS "Build Tests for the AES Crypt Engine" OFF)
endif()

# Option to control ability to install the library
option(aescrypt_engine_INSTALL "Install the AES Crypt Engine" ON)

# Determine whether clang-tidy will be performed
option(aescrypt_engine_CLANG_TIDY "Use clang-tidy to perform linting during build" OFF)

# Add the dependencies and source directories
add_subdirectory(dependencies)
add_subdirectory(src)

# Include testing support (defines BUILD_TESTING)
include(CTest)

# Build tests if conditions are met
if(BUILD_TESTING AND aescrypt_engine_BUILD_TESTS)
    add_subdirectory(test)
endif()
