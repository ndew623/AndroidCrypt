add_executable(test_aes_key_wrap test_aes_key_wrap.cpp)

target_link_libraries(test_aes_key_wrap PRIVATE Terra::libaes Terra::stf)

add_test(NAME test_aes_key_wrap
         COMMAND test_aes_key_wrap)

# Specify the C++ standard to observe
set_target_properties(test_aes_key_wrap
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

# Specify the compiler options
target_compile_options(test_aes_key_wrap
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wpedantic -Wextra -Wall>
        $<$<CXX_COMPILER_ID:MSVC>:>)
