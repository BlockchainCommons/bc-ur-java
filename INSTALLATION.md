# Installation for UR Java
This document gives the instruction for installing the Blockchain Commons UR Java library.

## Dependencies
We use [bc-ur](https://github.com/BlockchainCommons/bc-ur) as sub-module so make sure you clone this repo correctly.
Command to clone this repo along with all submodules
```console
$ git clone https://github.com/BlockchainCommons/bc-ur-java
$ cd bc-ur-java
$ git submodule init 
$ git submodule update --recursive
```

> We add utility script for installing all dependencies, you can find it at `<platform>/scripts/install_deps.sh`
If you want to do it manually by yourself, make sure all of following dependencies are installed correctly. 

[Adopt Open JDK 1.8](https://github.com/AdoptOpenJDK/openjdk8-binaries/releases) is recommended for both MacOS and Linux.
### Linux
> Well tested on Ubuntu 16.04, 18.04 and Debian 9, 10.

> Following packages can be installed via `apt-get`

- automake
- make

Make sure you have llvm/clang, libc++ and libc++abi installed. All of them with a minimum recommended version 10.

```console
$ wget https://apt.llvm.org/llvm.sh
$ chmod +x llvm.sh
$ sudo ./llvm.sh 10  # version 10

$ sudo apt-get install libc++-10-dev libc++abi-10-dev
```

### MacOS
> Following packages can be installed via `brew`

- automake
- make

## Android
> Working directory: `/android`

### Testing
```console
$ JAVA_HOME="your/java/home" ANDROID_SDK_ROOT="your/android-sdk/home" sudo -E ./gradlew clean connectedDebugAndroidTest
```

### Bundling
```console
$ JAVA_HOME="your/java/home" ANDROID_SDK_ROOT="your/android-sdk/home" sudo -E ./gradlew clean assembleRelease
```

> The `app-release.aar` file would be found in `app/build/outputs/aar`. You can compile it as a library in your project.

## Java (Web app/ Desktop app)
> Working directory: `/java`

### Build native libraries
Run following command for building the dynamic library file.
```console
$ JAVA_HOME="your/java/home" CC="clang-10" CXX="clang++-10" ./scripts/build.sh
```

> The dynamic library file would be found at `src/main/libs`. You need to install it into `java.library.path` for JVM can load it at runtime.

### Testing
```console
$ ./gradlew clean test
```

### Bundling
The `jar` file will be bundled by running
```console
$ ./gradlew clean assemble
```

> `jar` file just contain all `.class` files for running pure Java, no dynamic library is carried with.