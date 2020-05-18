# Cloning repository
```bash
bash> git clone git@github.com:dannftk/lmail.git
```
# Build requirements
- [cmake](https://cmake.org/) to configure the project. Minimum required version is __3.9.6__
- [conan](https://conan.io/) to download all the dependencies of the application and configure with a certain set of parameters. You can install __conan__ by giving a command to __pip__:
    ```bash
        bash> pip install --user conan
    ```
    To use __pip__ you need to install __python__ interpreter. I highly recommend to install __python3__-based versions in order to avoid unexpected results with __conan__

- A C++ compiler with __C++17__, __boost-1.73__ and __gtest-1.10__ support. The package has been successfully tested on compilation with __gcc-9.3__ (g++ with libstdc++11)

# Preparing with conan
First you need to set __conan's remote list__ to be able to download packages prescribed in the `conanfile.py` as requirements (dependencies). You need at least two remotes known by conan. We need __conan-center__ and __bincrafters__ repositories available. To check them if they already exist run the following command:
```bash
bash> conan remote list
```
If required remotes are already there you will see output alike:
```bash
bash> conan remote list
bincrafters: https://api.bintray.com/conan/bincrafters/public-conan [Verify SSL: True]
conan-center: https://conan.bintray.com [Verify SSL: True]
```
If one or both of them don't appear you should set them by running the commands:
```bash
bash> conan remote add conan-center https://conan.bintray.com
bash> conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
```
Since now you're ready to perform conan installation.

    WARNING: if you have variables CC or/and CXX set in your environment you need to unset them before executing next commands, otherwise, if conan decides to build a dependency on host the compiler selected from parameters and compiler from CC/CXX may contradict, as the result some cmake configuring processes while bulding dependencies may fail

If you have resolved all the possible issues described above, you can start installation with conan. Below there is installed a conan's environment with making use of __gcc-9.3__, __libstdc++11__, architecture __x86\_64__. If something does not correspond to your environment (for example, __gcc__ is a lower version), change it. Installation with __gcc-9.3__, __libstdc++11__, __x86\_64__, __Debug mode__:
```
bash> cd lmail
bash> conan install . -if debug/ -s arch=x86_64 -s arch_build=x86_64 -s compiler=gcc -s compiler.version=9.3 -s compiler.libcxx=libstdc++11 -s build_type=Debug --build missing
```
All the parameters provided to conan are vital. By using them conan determines whether it can download already built binary package from a remote or it must build a dependency up on the host by itself (if `--build missing` parameter is passed)
By the time the command has finished you will have got `debug` directory in the root of the project

To install prerequisites for __Release mode__ leaving other parameters untouched, use the following command:
```bash
bash> conan install . -if release/ -s arch=x86_64 -s arch_build=x86_64 -s compiler=gcc -s compiler.version=9.3 -s compiler.libcxx=libstdc++11 -s build_type=Release --build missing
```
As just the command has worked out `release` directory will appear in the root of the project

You can vary conan's parameters according to your needs. For instance, if you like to build a package with __gcc-6.5__ provide the `-s compiler.version=6.5`leaving all the rest parameters untouched

To learn more about conan available actions and parameters consult `conan --help`

# Configuring and building with cmake

Suppose, you have prepared __Debug mode__ with conan and got `debug` directory.

    WARNING: Before configuring make sure you don't have CC/CXX environment variables set, otherwise they may contradict with those having been configured by conan

To configure the project with `cmake` run the commands:
```bash
bash> cd debug
bash> cmake -DCMAKE_BUILD_TYPE=Debug -DINSTALL_DEFAULT_CONF=ON -DINSTALL_EMPTY_SCHEMA_DB=ON ../
```
The project is configured. To built it run:
```bash
bash> cmake --build .
```

`-DINSTALL_DEFAULT_CONF=ON` option provided tells to install default configuration file while installing. __WARNING:__ It may rewrite already existing configuration file

`-DINSTALL_EMPTY_SCHEMA_DB=ON` option provided tells to install an empty database schema with tables and their relationships. While installing __cmake__ will run the script that would interactively with you configure your enviroment to prepare the system use this database with lmail application installed. __WARNING:__ the script that will be run will ask you whether you like to substitute an old database (if any) by a new one, be careful, you are possible to leave that dangerous step and proceed

To enable building __unit and integrational tests__, provide an additional parameter `-DENABLE_TEST=ON` to __cmake__ while configuring

There are more parameters you can provide to cmake

If the compilation's finished successfully, in the directory `${project_root}/debug/bin/` you will find `lmail` binary. In case tests have been enabled and built you will also find `lmail_test` binary alongside

# Installation and environment configuration

To install the application in the syste, run:
```bash
bash> make install
```
If you need superuser rights, you need to run it under root:
```bash
bash> sudo make install
```
The latter depends on what the prefix you specified while configuring the project with __cmake__, __cmake__'s variable with prefix is `-DCMAKE_INSTALL_PREFIX=/path/to/your/installation/directory`. This variable is set with one of the system path by default, but you always can change it what you want it to be

# Automatic configuration

If you don't like bothering youself with selecting a conan configuration you could try to run __cmake__ with the flag specifying to configure conan automatically. It will take your default conan profile and try to apply to the project. To enable this you should run the following command with preliminarily created the build directory:
```bash
bash> mkdir build
bash> cd build
bash> cmake -DGEN_CONAN_BUILD_INFO=ON -DCMAKE_BUILD_TYPE=Debug -DINSTALL_DEFAULT_CONF=ON -DINSTALL_EMPTY_SCHEMA_DB=ON ../
```
This will try to generate conan and cmake info automatically and configure the project in __Debug__ mode

# Afterwords

To configure project in __Release mode__ provide `-DCMAKE_BUILD_TYPE=Release` instead of `-DCMAKE_BUILD_TYPE=Debug`

__Release__ version accomplishes better performance results

Gotcha!

If you have any questions do not hesitate to ask [me](mailto:dannftk@yandex.ru)
