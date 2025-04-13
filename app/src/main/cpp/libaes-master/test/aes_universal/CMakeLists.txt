add_executable(test_aes_universal test_aes_universal.cpp)

target_include_directories(test_aes_universal
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(test_aes_universal PRIVATE Terra::libaes Terra::stf)

add_test(NAME test_aes_universal
         COMMAND test_aes_universal)

# Specify the C++ standard to observe
set_target_properties(test_aes_universal
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

# Specify the compiler options
target_compile_options(test_aes_universal
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wpedantic -Wextra -Wall>
        $<$<CXX_COMPILER_ID:MSVC>:>)

# If speed tests are enabled, pass that to the compiler
if(TERRA_ENABLE_AES_SPEED_TESTS)
    target_compile_definitions(test_aes_universal PRIVATE TERRA_ENABLE_AES_SPEED_TESTS)
endif()
