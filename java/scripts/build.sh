#!/bin/bash

set -e

source scripts/helper.sh

echo "${JAVA_HOME:?}"
echo "${CC:?}"
echo "${CXX:?}"

echo 'Cleanup...'
./scripts/cleanup.sh

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

# Install base bc-ur lib
pushd "$ROOT_DIR"/deps/bc-ur
./configure
make clean
make CPPFLAGS=-fPIC check
popd

# Install jni bc-ur lib
echo "Building $LIB_NAME..."
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
echo "Done. Checkout the release file at $OUT_DIR/$LIB_NAME"
