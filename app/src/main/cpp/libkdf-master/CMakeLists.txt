cmake_minimum_required(VERSION 3.21)

# Define the Key Derivation Function (KDF) project
project(libkdf
        VERSION 1.0.8.0
        DESCRIPTION "Key Derivation Function (KDF) Library"
        LANGUAGES CXX)

# Set options depending on whether this is a subproject
if(PROJECT_IS_TOP_LEVEL)
    # Option to control whether tests are built
    option(libkdf_BUILD_TESTS "Build Tests for the KDF Library" ON)
else()
    # Option to control whether tests are built
    option(libkdf_BUILD_TESTS "Build Tests for the KDF Library" OFF)
endif()

# Option to control ability to install the library
option(libkdf_INSTALL "Install the KDF Library" ON)

# Determine whether clang-tidy will be performed
option(libkdf_CLANG_TIDY "Use clang-tidy to perform linting during build" OFF)

add_subdirectory(dependencies)
add_subdirectory(src)

include(CTest)

if(BUILD_TESTING AND libkdf_BUILD_TESTS)
    add_subdirectory(test)
endif()
