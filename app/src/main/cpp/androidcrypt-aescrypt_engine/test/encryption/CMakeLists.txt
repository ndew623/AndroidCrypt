# Create the test executable
add_executable(test_encryption test_encryption.cpp)

# Link to the required libraries
target_link_libraries(test_encryption Terra::aescrypt_engine Terra::stf)

# Specify the C++ standard to observe
set_target_properties(test_encryption
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

# Specify the compiler options
target_compile_options(test_encryption
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wpedantic -Wextra -Wall>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>)

# Ensure CTest can find the test
add_test(NAME test_encryption
         COMMAND test_encryption)
