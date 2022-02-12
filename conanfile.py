# how to use semver https://devhints.io/semver

from conans import ConanFile, CMake, tools

class LMail(ConanFile):
    name = "lmail"
    version = "1.5.0"
    author = "Denis Pronin"
    url = "https://github.com/dpronin/lmail"
    description = "A tool for mailing in a local host"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake_find_package"

    scm = {
        "type": "git",
        "subfolder": name,
        "url": "auto",
        "revision": "auto",
        "username": "git"
    }

    def requirements(self):
        self.requires("boost/[~1.78]")
        self.requires("cryptopp/[~8.6]")
        self.requires("readline/[~8.1]")
        self.requires("sqlite3/[~3.37]")

    def build_requirements(self):
        self.build_requires("gtest/[~1.11]")

    def _configure(self, verbose = True):
        cmake = CMake(self)
        cmake.verbose = verbose
        cmake.definitions['CMAKE_BUILD_TYPE'] = "Debug" if self.settings.build_type == "Debug" else "Release"
        cmake.definitions['ENABLE_GDB_SYMBOLS'] = self.settings.build_type == "Debug"
        cmake.configure(source_folder = self.name)
        return cmake

    def build(self):
        cmake = self._configure()
        cmake.build()
        cmake.test(output_on_failure = True)

    def package(self):
        cmake = self._configure()
        cmake.install()
