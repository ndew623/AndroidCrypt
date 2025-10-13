# Include standard installation directory variables
include(GNUInstallDirs)

# Threading support is required
find_package(Threads REQUIRED)

# Create the library
add_library(aescrypt_engine STATIC decryptor.cpp encryptor.cpp)
add_library(Terra::aescrypt_engine ALIAS aescrypt_engine)

# Make project include directory available to external projects
target_include_directories(aescrypt_engine
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Specify the C++ standard to observe
set_target_properties(aescrypt_engine
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

# If requesting clang-tidy, try to look for it
if(aescrypt_engine_CLANG_TIDY)
    find_program(CLANG_TIDY_COMMAND NAMES "clang-tidy")
    if(CLANG_TIDY_COMMAND)
        set_target_properties(aescrypt_engine PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
    else()
        message(WARNING "Could not find clang-tidy")
    endif()
endif()

# Use the following compile options
target_compile_options(aescrypt_engine
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wpedantic -Wextra -Wall -Werror>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /Zc:__cplusplus>)

# Link against dependencies
target_link_libraries(aescrypt_engine
    PRIVATE
        Terra::libaes
        Terra::secutil
        Terra::charutil
        Terra::kdf
        Terra::random
    PUBLIC
        Terra::logger)

# Install target and associated include files
if(aescrypt_engine_INSTALL)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ TYPE INCLUDE)
    install(TARGETS aescrypt_engine EXPORT aescrypt_engineTargets ARCHIVE)
    install(EXPORT aescrypt_engineTargets
            FILE aescrypt_engineConfig.cmake
            NAMESPACE Terra::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/aescrypt_engine)
endif()
