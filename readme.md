# Project building

## Build requirements

- [cmake](https://cmake.org/) to configure the project. Minimum required version is __3.12__ unless [_cmake presets_](https://cmake.org/cmake/help/v3.19/manual/cmake-presets.7.html) feature is going to be used requiring at least __3.19__
- [conan 2.0](https://conan.io/) to download all the dependencies of the application and configure with a certain set of parameters. You can install __conan__ by giving a command to __pip__. To use __pip__ you need to install __python__ interpreter. I highly recommend to install a __python3__-based version and as the result use __pip3__ in order to avoid unexpected results with __conan__

To install/upgrade __conan__ within system's python environment for a current linux's user give the command:

```bash
$ pip3 install --user conan --upgrade
```

- A C++ compiler with at least __C++23__ support

## Preparing conan

First you need to set __conan's remote list__ to be able to download packages prescribed in the _conanfile.py_ as requirements (dependencies). You need at least one default remote known by conan. We need at least __conancenter__ repository available. To check if it already exists run the following command:
```bash
$ conan remote list
```
If required remote is already there you will see output alike:
```bash
$ conan remote list
conancenter: https://center.conan.io [Verify SSL: True, Enabled: True]
```
If it doesn't appear you should install it by running the command:
```bash
$ conan remote add conancenter https://center.conan.io
```

## Pull out

```bash
$ git clone git@github.com:dpronin/lmail.git
```

## Configure, build and run

> :information_source: **conan** has [profiles](https://docs.conan.io/2.0/reference/config_files/profiles.html) to predefine options and environment variables in order to not provide them any time within the command line interface. To learn more about conan available actions and parameters consult `conan --help`. Also reach out to the [conan documentation](https://docs.conan.io/2/)

### Conan profile

Profile **default** used below might look like as the following:

```ini
[settings]
arch=x86_64
build_type=Release
compiler=gcc
compiler.libcxx=libstdc++11
compiler.version=13.2
os=Linux

[buildenv]
CXX=g++
CC=gcc
```

# Configuring and building with conan and cmake

Suppose, you have prepared __Debug mode__ with conan and got `build/Debug` directory.

    WARNING: Before configuring make sure you don't have CC/CXX environment variables set, otherwise they may contradict with those having been configured by conan

To configure the project with `cmake` run the commands:

```bash
$ cd lmail
$ conan install -s build_type=Debug -pr default --build=missing --update -of out/default .
$ source out/default/build/Debug/generators/conanbuild.sh
$ cmake --preset conan-debug
$ cmake --build --preset conan-debug --parallel $(nproc)
$ source out/default/build/Debug/generators/deactivate_conanbuild.sh
```

`-DINSTALL_DEFAULT_CONF=ON` option provided tells to install default configuration file while installing. __WARNING:__ It may rewrite already existing configuration file

`-DINSTALL_EMPTY_SCHEMA_DB=ON` option provided tells to install an empty database schema with tables and their relationships. While installing __cmake__ will run the script that would interactively with you configure your enviroment to prepare the system use this database with lmail application installed. __WARNING:__ the script that will be run will ask you whether you like to substitute an old database (if any) by a new one, be careful, you are possible to leave that dangerous step and proceed

To enable building __unit and integrational tests__, provide an additional parameter `-DBUILD_TESTING=ON` to __cmake__ while configuring

There are more parameters you can provide to cmake

If the compilation's finished successfully, in the directory `${project_root}/debug/bin/` you will find `lmail` binary. In case tests have been enabled and built you will also find `lmail_test` binary alongside

# Installation and environment configuration

To install the application in the system, run:
```bash
bash> cmake --install out/default/build/Debug --config Debug
```

If you need superuser rights, you need to run it under root:
```bash
bash> sudo cmake --install out/default/build/Debug --config Debug
```

The latter depends on what the prefix you specified while configuring the project with __cmake__, __cmake__'s variable with prefix is `-DCMAKE_INSTALL_PREFIX=/path/to/your/installation/directory`. This variable is set with one of the system path by default, but you always can change it what you want it to be

# Automatic configuration

If you don't like bothering youself with selecting a conan configuration you could try to run __cmake__ with the flag specifying to configure conan automatically. It will take your default conan profile and try to apply to the project. To enable this you should run the following command with preliminarily created the build directory:
```bash
bash> ...
bash> cmake --preset conan-debug -DGEN_CONAN_BUILD_INFO=ON -DINSTALL_DEFAULT_CONF=ON -DINSTALL_EMPTY_SCHEMA_DB=ON ../
bash> ...
```
This will try to generate conan and cmake info automatically and configure the project in __Debug__ mode

# Afterwords

To configure and build the project in __Release mode__:

```bash
$ cd lmail
$ conan install -s build_type=Debug -pr default --build=missing --update -of out/default .
$ source out/default/build/Debug/generators/conanbuild.sh
$ cmake --preset conan-debug
$ cmake --build --preset conan-debug --parallel $(nproc)
$ source out/default/build/Debug/generators/deactivate_conanbuild.sh
```

__Release__ version accomplishes better performance results

Gotcha!

If you have any questions do not hesitate to ask [me](mailto:dannftk@yandex.ru)
