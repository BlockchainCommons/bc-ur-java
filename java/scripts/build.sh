#!/bin/bash

set -e

source scripts/helper.sh

echo "${JAVA_HOME:?}"
echo "${CC:?}"
echo "${CXX:?}"

ROOT_DIR=$(
  cd ..
  pwd
)

LIB_NAME="libbc-ur.dylib"
OUT_DIR=src/main/libs
JNI_MD_DIR="darwin"
if ! is_osx; then
  LIB_NAME="libbc-ur.so"
  JNI_MD_DIR="linux"
fi
BUILD_LOG_DIR="build/log"
BUILD_LOG="$BUILD_LOG_DIR/$(date +%s)-log.txt"

clean_up() {
  ./scripts/cleanup.sh
}

build_bc_ur() {
  pushd "$ROOT_DIR"/deps/bc-ur
  ./configure
  make clean
  make CPPFLAGS=-fPIC check
  popd
}

build_jni() {
  mkdir -p $OUT_DIR
  $CXX \
    -I"$JAVA_HOME/include" \
    -I"$JAVA_HOME/include/$JNI_MD_DIR" \
    -I"$ROOT_DIR/deps/bc-ur/src" \
    -fexceptions -frtti -std=c++17 -stdlib=libc++ -shared -fPIC \
    src/main/jniLibs/bc-ur.cpp \
    "$ROOT_DIR"/deps/bc-ur/src/libbc-ur.a \
    -o \
    $OUT_DIR/$LIB_NAME
}

(
  mkdir -p ${BUILD_LOG_DIR}
  echo -n >"${BUILD_LOG}"

  echo 'Cleanup...'
  clean_up

  echo 'Building bc-ur...'
  build_bc_ur

  echo "Building $LIB_NAME..."
  build_jni
  echo "Done. Checkout the release file at $OUT_DIR/$LIB_NAME"
) | tee "${BUILD_LOG}"
