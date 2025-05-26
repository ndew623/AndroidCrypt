# Create the test excutable
add_executable(test_pbkdf1 test_pbkdf1.cpp)

# Link to the required libraries
target_link_libraries(test_pbkdf1 Terra::kdf Terra::stf)

# Specify the C++ standard to observe
set_target_properties(test_pbkdf1
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

# Specify the compiler options
target_compile_options(test_pbkdf1
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wpedantic -Wextra -Wall>
        $<$<CXX_COMPILER_ID:MSVC>: >)

# Ensure CTest can find the test
add_test(NAME test_pbkdf1
         COMMAND test_pbkdf1)
