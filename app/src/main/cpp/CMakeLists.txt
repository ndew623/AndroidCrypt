
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # Enforce the standard

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("androidcrypt")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
#
# In this top level CMakeLists.txt, ${CMAKE_PROJECT_NAME} is used to define
# the target library name; in the sub-module's CMakeLists.txt, ${PROJECT_NAME}
# is preferred for the same purpose.
#
# In order to load a library into your app from Java/Kotlin, you must call
# System.loadLibrary() and pass the name of the library defined here;
# for GameActivity/NativeActivity derived applications, the same library name must be
# used in the AndroidManifest.xml file.
add_library(${CMAKE_PROJECT_NAME} SHARED
    # List C/C++ source files with relative paths to this CMakeLists.txt.
    androidcrypt.cpp)

#AESCrypt
option(aescrypt_engine_INSTALL "Install the AES Crypt Engine" OFF)
option(aescrypt_engine_CLANG_TIDY "Use clang-tidy to perform linting during build" OFF)
option(aescrypt_engine_BUILD_TESTS "Build Tests for the AES Crypt Engine" OFF)

add_subdirectory(conio-master)
add_subdirectory(logger-master)
add_subdirectory(libaes-master)
add_subdirectory(libhash-master)
add_subdirectory(libkdf-master)
add_subdirectory(dependencies)
add_subdirectory(aescrypt_engine-master)


# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.
target_link_libraries(${CMAKE_PROJECT_NAME}
        PRIVATE
        Terra::aescrypt_engine
        Terra::secutil
        Terra::charutil
        Terra::kdf
        Terra::random
        PUBLIC
        android
        log
        Terra::logger
        Terra::libhash)
