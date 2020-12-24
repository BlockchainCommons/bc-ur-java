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

get_filename_component(ROOT_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)

# define source files
file(GLOB BC_UR_SRC ${ROOT_DIR}/deps/bc-ur/src/*.c*)
set(TARGET_INCLUDE_DIRS ${ROOT_DIR}/deps/bc-ur/src)

# define include dir
include_directories(/usr/local/include)

# compile
add_library(bc-ur-base STATIC ${BC_UR_SRC})

add_library(bc-ur SHARED ${ROOT_DIR}/java/src/main/jniLibs/bc-ur.cpp)

# link
target_link_libraries(bc-ur bc-ur-base)

# target headers
target_include_directories(bc-ur PRIVATE ${TARGET_INCLUDE_DIRS})