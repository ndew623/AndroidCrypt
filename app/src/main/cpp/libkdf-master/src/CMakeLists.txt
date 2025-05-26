# Create the KDF library
add_library(kdf STATIC ackdf.cpp pbkdf1.cpp pbkdf2.cpp hkdf.cpp)
add_library(Terra::kdf ALIAS kdf)

# Make project include directory available to external projects
target_include_directories(kdf
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)

# Specify the C++ standard to observe
set_target_properties(kdf
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

# If requesting clang-tidy, try to look for it
if(libkdf_CLANG_TIDY)
    find_program(CLANG_TIDY_COMMAND NAMES "clang-tidy")
    if(CLANG_TIDY_COMMAND)
        set_target_properties(kdf PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
    else()
        message(WARNING "Could not find clang-tidy")
    endif()
endif()

# Use the following compile options
target_compile_options(kdf
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wpedantic -Wextra -Wall -Werror>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>)

# Link against library dependencies
target_link_libraries(kdf
    PRIVATE
        Terra::secutil
    PUBLIC
        Terra::libhash)

# Install target and associated include files
if(libkdf_INSTALL)
    include(GNUInstallDirs)
    install(TARGETS kdf EXPORT kdfTargets ARCHIVE)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ TYPE INCLUDE)
    install(EXPORT kdfTargets
            FILE kdfConfig.cmake
            NAMESPACE Terra::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/kdf)
endif()
