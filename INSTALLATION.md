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

> We add utility script for installing all dependencies, you can find it at `java/scripts/install_deps.sh`
If you want to do it manually by yourself, make sure all of following dependencies are installed correctly. 

### Linux
> Following packages can be installed via `apt-get`

- automake
- make
- libc++-10-dev (or above)
- libc++abi-10-dev (or above)
- openjdk-8-jdk (or above)
- clang-10 (or above)

### MacOS
> Following packages can be installed via `brew`
- automake
- make

[Adopt Open JDK 1.8](https://github.com/AdoptOpenJDK/openjdk8-binaries/releases) is recommended for MacOS.
## Android
> Working directory: `/android`

### Testing
```console
./gradlew clean connectedDebugAndroidTest
```

### Bundling
```console
./gradlew clean assembleRelease
```

> The `app-release.aar` file would be found in `app/build/outputs/aar`. You can compile it as a library in your project.

## Java (Web app/ Desktop app)
> Working directory: `/java`

### Build native libraries
Run following command for building the dynamic library file.
```console
./scripts/build.sh
```

> If you are working on mac OS, we recommend run the command with `sudo` permission.

> The dynamic library file would be found at `src/main/libs`. You need to install it into `java.library.path` for JVM can load it at runtime.

### Testing
```console
./gradlew clean test
```

### Bundling
The `jar` file will be bundled by running
```console
./gradlew clean assemble
```

> `jar` file just contain all `.class` files for running pure Java, no dynamic library is carried with.