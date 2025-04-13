# Create the library
add_library(aes STATIC
    aes.cpp
    aes_intel.cpp
    aes_universal.cpp
    aes_key_wrap.cpp
    cpu_check.cpp)
add_library(Terra::libaes ALIAS aes)

# Specify the internal and public include directories
target_include_directories(aes
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)

# Specify the C++ standard to observe
set_target_properties(aes
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

# Use the following compile options
target_compile_options(aes
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wpedantic -Wextra -Wall>
        $<$<CXX_COMPILER_ID:MSVC>:>)

# Ensure compiler knows if requested to build with Intel Intrinsics
if(TERRA_ENABLE_INTEL_INTRINSICS)
    # Set the compiler definition
    target_compile_definitions(aes PRIVATE TERRA_ENABLE_INTEL_INTRINSICS)

    # Set compiler options for some platforms to include -maes
    if(DEFINED CMAKE_OSX_ARCHITECTURES AND NOT CMAKE_OSX_ARCHITECTURES STREQUAL "")
        target_compile_options(aes PRIVATE
            # On Mac, -maes for explicit single x86_64
            $<$<AND:$<PLATFORM_ID:Darwin>,$<STREQUAL:${CMAKE_OSX_ARCHITECTURES},x86_64>>:-maes>
            # On Mac, -maes and warning suppression for universal with x86_64 (excludes single x86_64)
            $<$<AND:$<PLATFORM_ID:Darwin>,$<IN_LIST:x86_64,${CMAKE_OSX_ARCHITECTURES}>,$<NOT:$<STREQUAL:${CMAKE_OSX_ARCHITECTURES},x86_64>>:-maes -Wno-unused-command-line-argument>)
    else()
        target_compile_options(aes PRIVATE
            # On any platform, -maes for system processors x86_64 or i386
            $<$<OR:$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},x86_64>,$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},i386>>:-maes>)
    endif()
endif()

# Link against library dependencies
target_link_libraries(aes PRIVATE Terra::secutil Terra::bitutil)

# If requesting clang-tidy, try to look for it
if(libaes_CLANG_TIDY)
    find_program(CLANG_TIDY_COMMAND NAMES "clang-tidy")
    if(CLANG_TIDY_COMMAND)
        set_target_properties(aes PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
    else()
        message(WARNING "Could not find clang-tidy")
    endif()
endif()

# Install target and associated include files
if(libaes_INSTALL)
    include(GNUInstallDirs)
    install(TARGETS aes EXPORT libaesTargets ARCHIVE)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ TYPE INCLUDE)
    install(EXPORT libaesTargets
            FILE libaesConfig.cmake
            NAMESPACE Terra::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libaes)
endif()
