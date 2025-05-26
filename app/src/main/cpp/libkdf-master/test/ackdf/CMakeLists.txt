# Create the test excutable
add_executable(test_ackdf test_ackdf.cpp)

# Link to the required libraries
target_link_libraries(test_ackdf Terra::kdf Terra::stf)

# Specify the C++ standard to observe
set_target_properties(test_ackdf
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

# Specify the compiler options
target_compile_options(test_ackdf
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wpedantic -Wextra -Wall>
        $<$<CXX_COMPILER_ID:MSVC>: >)

# Ensure CTest can find the test
add_test(NAME test_ackdf
         COMMAND test_ackdf)
