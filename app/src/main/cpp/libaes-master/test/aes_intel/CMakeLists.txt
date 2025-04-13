add_executable(test_aes_intel test_aes_intel.cpp)

target_include_directories(test_aes_intel
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(test_aes_intel PRIVATE Terra::libaes Terra::stf)

add_test(NAME test_aes_intel
         COMMAND test_aes_intel)

# Specify the C++ standard to observe
set_target_properties(test_aes_intel
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

# Specify the compiler options
target_compile_options(test_aes_intel
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wpedantic -Wextra -Wall>
        $<$<CXX_COMPILER_ID:MSVC>:>)

# Ensure compiler knows if requested to build with Intel Intrinsics
if(TERRA_ENABLE_INTEL_INTRINSICS)
    target_compile_definitions(test_aes_intel PRIVATE TERRA_ENABLE_INTEL_INTRINSICS)
endif()

# If speed tests are enabled, pass that to the compiler
if(TERRA_ENABLE_AES_SPEED_TESTS)
    target_compile_definitions(test_aes_intel PRIVATE TERRA_ENABLE_AES_SPEED_TESTS)
endif()
