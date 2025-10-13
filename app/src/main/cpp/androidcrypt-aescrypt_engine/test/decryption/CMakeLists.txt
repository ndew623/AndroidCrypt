# Create the test executable
add_executable(test_decryption
    test_stream_0.cpp
    test_stream_1.cpp
    test_stream_2.cpp
    test_stream_3.cpp)

# Link to the required libraries
target_link_libraries(test_decryption Terra::aescrypt_engine Terra::stf)

# Specify the C++ standard to observe
set_target_properties(test_decryption
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

# Specify the compiler options
target_compile_options(test_decryption
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wpedantic -Wextra -Wall>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>)

# Ensure CTest can find the test
add_test(NAME test_decryption
         COMMAND test_decryption)
