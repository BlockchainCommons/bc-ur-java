#!/bin/bash

source scripts/helper.sh

lib_name="libbc-ur.dylib"
out_dir=build/release
jni_md_dir="darwin"
if is_osx; then
  java_home=$(/usr/libexec/java_home 2>/dev/null)
  export JAVA_HOME=$java_home
  export CC="clang"
  export CXX="clang++"
else
  lib_name="libbc-ur.so"
  jni_md_dir="linux"

  export CC="clang-10"
  export CXX="clang++-10"
  if [ "$JAVA_HOME" == "" ]; then
    export JAVA_HOME="/usr/lib/jvm/java-8-openjdk-amd64"
  fi
fi

# Install base bc-ur lib
pushd ../deps/bc-ur || exit
./configure
make clean
make CPPFLAGS=-fPIC check
popd || exit

# Install jni bc-ur lib
echo "Building $lib_name..."
mkdir -p $out_dir
$CXX -I$JAVA_HOME/include -I$JAVA_HOME/include/$jni_md_dir -fexceptions -frtti -std=c++17 -stdlib=libc++ -shared -fPIC src/main/jniLibs/bc-ur.cpp ../deps/bc-ur/src/libbc-ur.a -o $out_dir/$lib_name || exit
echo "Done. Checkout the release file at $out_dir/$lib_name"
