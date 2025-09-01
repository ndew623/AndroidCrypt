add_executable(test_sha384 test_sha384.cpp)

target_link_libraries(test_sha384 Terra::libhash Terra::bitutil Terra::stf)

# Specify the C++ standard to observe
set_target_properties(test_sha384
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

# Use the following compile options
target_compile_options(test_sha384
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wpedantic -Wextra -Wall>
        $<$<CXX_COMPILER_ID:MSVC>: >)

add_test(NAME test_sha384
         COMMAND test_sha384)
