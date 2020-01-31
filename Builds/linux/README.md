# Linux Build and Run Instructions

This document focuses on building ripple-alpha-core for development purposes under recent
Ubuntu linux distributions. 

Note: Ubuntu 16.04 users may need to update their compiler (see the dependencies
section). For non Ubuntu distributions, the steps below should work be
installing the appropriate dependencies using that distribution's package
management tools.

## Dependencies

gcc-7 or later is required.

Use `apt-get` to install the dependencies provided by the distribution

```
$ sudo apt-get update
$ sudo apt-get install -y gcc g++ wget git cmake protobuf-compiler libprotobuf-dev libssl-dev
```

Advanced users can choose to install newer versions of gcc, or the clang compiler.
At this time, ripple-alpha-core only supports protobuf version 2. Using version 3 of 
protobuf will give errors.

### Build Boost

Boost 1.71 or later is required. We recommend downloading and compiling boost
with the following process: After changing to the directory where
you wish to download and compile boost, run

``` 
$ wget https://dl.bintray.com/boostorg/release/1.71.0/source/boost_1_71_0.tar.gz
$ tar -xzf boost_1_71_0.tar.gz
$ cd boost_1_71_0
$ ./bootstrap.sh
$ ./b2 headers
$ ./b2 -j 4
```

### (Optional) Dependencies for Building Source Documentation

Source code documentation is not required for running/debugging ripple-alpha-core. That
said, the documentation contains some helpful information about specific
components of the application. For more information on how to install and run
the necessary components, see [this document](../../docs/README.md)

## Build

### Clone the ripple-alpha-core repository

From a shell:

```
git clone https://github.com/ripple-alpha/ripple-alpha-core.git
cd ripple-alpha-core
```

For a stable release, choose the `master` branch or one of the tagged releases
listed on [GitHub](https://github.com/ripple-alpha/ripple-alpha-core/releases). 

```
git checkout master
```

### Configure Library Paths

If you didn't persistently set the `BOOST_ROOT` environment variable to the
directory in which you compiled boost, then you should set it temporarily.

For example, you built Boost in your home directory `~/boost_1_71_0`, you
would do for any shell in which you want to build:

```
export BOOST_ROOT=~/boost_1_71_0
source ~/.profile
```

Alternatively, you can add `DBOOST_ROOT=~/boost_1_71_0` to the command line when
invoking `cmake`.

### Generate Configuration

All builds should be done in a separate directory from the source tree root 
(a subdirectory is fine). For example, from the root of the ripple-alpha-core source tree:

```
mkdir my_build
cd my_build
```

followed by:

```
cmake -DCMAKE_BUILD_TYPE=Release ..
```

If your operating system does not provide static libraries (Arch Linux, and 
Manjaro Linux, for example), you must configure a non-static build by adding
`-Dstatic=OFF` to the above cmake line.

`CMAKE_BUILD_TYPE` can be changed as desired for `Debug` vs.
`Release` builds (all four standard cmake build types are supported).

To select a different compiler (most likely gcc will be found by default), pass 
`-DCMAKE_C_COMPILER=<path/to/c-compiler>` and
`-DCMAKE_CXX_COMPILER=</path/to/cxx-compiler>` when configuring. If you prefer, 
you can instead set `CC` and `CXX` environment variables which cmake will honor.

#### Options During Configuration:

The CMake file defines a number of configure-time options which can be
examined by running `cmake-gui` or `ccmake` to generated the build. In
particular, the `unity` option allows you to select between the unity and
non-unity builds. `unity` builds are faster to compile since they combine
multiple sources into a single compiliation unit - this is the default if you
don't specify. `nounity` builds can be helpful for detecting include omissions
or for finding other build-related issues, but aren't generally needed for
testing and running.

* `-Dunity=ON` to enable/disable unity builds (defaults to ON)  
* `-Dassert=ON` to enable asserts
* `-Djemalloc=ON` to enable jemalloc support for heap checking
* `-Dsan=thread` to enable the thread sanitizer with clang
* `-Dsan=address` to enable the address sanitizer with clang
* `-Dstatic=ON` to enable static linking library dependencies

Several other infrequently used options are available - run `ccmake` or
`cmake-gui` for a list of all options.

### Build

Once you have generated the build system, you can run the build via cmake:

```
cmake --build . -- -j <parallel jobs>
```

the `-j` parameter in this example tells the build tool to compile several
files in parallel. This value should be chosen roughly based on the number of
cores you have available and/or want to use for building.

When the build completes succesfully, you will have a `ripple-alpha-core` executable in
the current directory, which can be used to connect to the network (when
properly configured) or to run unit tests.


#### Optional Installation

The ripple-alpha-core cmake build supports an installation target that will install
ripple-alpha-core as well as a support library that can be used to sign transactions. In
order to build and install the files, specify the `install` target when
building, e.g.:

```
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/opt/local ..
cmake --build . --target install -- -j <parallel jobs>
```

We recommend specifying `CMAKE_INSTALL_PREFIX` when configuring in order to
explicitly control the install location for your files. Without this setting,
cmake will typically install in `/usr/local`. It is also possible to "rehome"
the installation by specifying the `DESTDIR` env variable during the install phase,
e.g.:

```
DESTDIR=~/mylibs cmake --build . --target install -- -j <parallel jobs>
```

in which case, the files would be installed in the `CMAKE_INSTALL_PREFIX` within
the specified `DESTDIR` path.

## Unit Tests (Recommended)

`ripple-alpha-core` builds a set of unit tests into the server executable. To run these unit
tests after building, pass the `--unittest` option to the compiled `ripple-alpha-core`
executable. The executable will exit with summary info after running the unit tests.

## Configure

Complete the following configurations that are required for ripple-alpha-core to start up successfully. All other configuration is optional and can be tweaked after you have a working server.

Create a copy of the example config file (assumes you're in the ripple-alpha-core folder already). Saving the config file to this location enables you to run ripple-alpha-core as a non-root user (recommended).

```
cd /path/to/ripple-alpha-core
sudo mkdir -p /opt/ripple-alpha/etc/
sudo cp cfg/ripple-alpha-core-example.cfg /opt/ripple-alpha/etc/ripple-alpha-core.cfg
```

Edit the config file to set necessary file paths. The user you plan to run ripple-alpha-core as must have write permissions to all of the paths you specify here.

Set the [node_db]'s path to the location where you want to store the ledger database.

Set the [database_path] to the location where you want to store other database data. (This includes an SQLite database with configuration data, and is typically one level above the [node_db] path field.)

Set the [debug_logfile] to a path where ripple-alpha-core can write logging information.

Copy the example validators.txt file to the same folder as ripple-alpha-core.cfg:
```
sudo cp cfg/validators-example.txt /opt/ripple-alpha/etc/validators.txt
```

Create symbolic links and create user.
```
sudo useradd ripple-alpha-core
sudo mkdir -p /opt/ripple-alpha/bin/
sudo mkdir -p /etc/opt/ripple-alpha/
sudo cp -r my_build/ripple-alpha-core /opt/ripple-alpha/bin/
sudo chown ripple-alpha-core:ripple-alpha-core /opt/ripple-alpha/bin/ripple-alpha-core 
sudo ln -s /opt/ripple-alpha/bin/ripple-alpha-core /usr/local/bin/ripple-alpha-core
sudo ln -s /opt/ripple-alpha/etc/ripple-alpha-core.cfg /etc/opt/ripple-alpha/
sudo ln -s /opt/ripple-alpha/etc/validators.txt /etc/opt/ripple-alpha/
```

## Run

To run your stock ripple-alpha-core server from the executable you built, using the configurations you defined:

```
sudo ripple-alpha-core
```
