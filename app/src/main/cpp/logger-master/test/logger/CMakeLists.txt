# Create the test excutable
add_executable(test_logger test_logger.cpp)

# Link to the required libraries
target_link_libraries(test_logger Terra::logger Terra::conio Terra::stf)

# Specify the C++ standard to observe
set_target_properties(test_logger
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

# Specify the compiler options
target_compile_options(test_logger
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wpedantic -Wextra -Wall>
        $<$<CXX_COMPILER_ID:MSVC>: >)

# Ensure CTest can find the test
add_test(NAME test_logger
         COMMAND test_logger)
