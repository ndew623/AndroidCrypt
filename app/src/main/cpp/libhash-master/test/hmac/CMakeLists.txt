add_executable(test_hmac test_hmac.cpp)

target_link_libraries(test_hmac Terra::libhash Terra::stf)

# Specify the C++ standard to observe
set_target_properties(test_hmac
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

# Use the following compile options
target_compile_options(test_hmac
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wpedantic -Wextra -Wall>
        $<$<CXX_COMPILER_ID:MSVC>: >)

add_test(NAME test_hmac
         COMMAND test_hmac)
