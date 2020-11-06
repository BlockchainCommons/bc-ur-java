# Sets the minimum version of CMake required to build your native library.
# This ensures that a certain set of CMake features is available to
# your build.

cmake_minimum_required(VERSION 3.4.1)
project(bc-ur)

# Specifies a library name, specifies whether the library is STATIC or
# SHARED, and provides relative paths to the source code. You can
# define multiple libraries by adding multiple add_library() commands,
# and CMake builds them for you. When you build your app, Gradle
# automatically packages shared libraries with your APK.

file(GLOB_RECURSE BC_UR_SRC "../deps/bc-ur/src/*.c*")

include_directories(../deps/bc-ur/src /usr/local/include)

add_library(bc-ur-base STATIC ${BC_UR_SRC})

add_library(bc-ur SHARED ../java/src/main/jniLibs/bc-ur.cpp)

target_link_libraries(bc-ur bc-ur-base)