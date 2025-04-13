# Create the test excutable
add_executable(logger_demo logger_demo.cpp)

# Link to the required libraries
target_link_libraries(logger_demo Terra::logger)

# Specify the C++ standard to observe
set_target_properties(logger_demo
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

# Specify the compiler options
target_compile_options(logger_demo
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wpedantic -Wextra -Wall>
        $<$<CXX_COMPILER_ID:MSVC>: >)
