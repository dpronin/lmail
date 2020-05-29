# how to use semver https://devhints.io/semver

from conans import ConanFile, CMake, tools

class LMail(ConanFile):
    name = "lmail"
    version = "0.3.1"
    author = "Denis Pronin"
    url = "https://github.com/dannftk/lmail"
    description = "A tool for mailing in a local host"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"

    requires = "boost/[~1.73]", \
        "sqlite_orm/[~1.4]@bincrafters/stable", \
        "cryptopp/[~8.2]@bincrafters/stable", \
        "readline/[~7.0]@bincrafters/stable"

    build_requires = \
        "gtest/[~1.10]"

    scm = {
        "type": "git",
        "subfolder": name,
        "url": "auto",
        "revision": "auto",
        "username": "git"
    }

    def _configure(self, enable_test = False, verbose = True):
        cmake = CMake(self)
        cmake.verbose = verbose
        cmake.definitions['CMAKE_BUILD_TYPE'] = "Debug" if self.settings.build_type == "Debug" else "Release"
        cmake.definitions['ENABLE_TEST'] = enable_test
        cmake.definitions['ENABLE_GDB_SYMBOLS'] = self.settings.build_type == "Debug"
        cmake.configure(source_folder = self.name)
        return cmake

    def build(self):
        cmake = self._configure(enable_test = True)
        cmake.build()
        cmake.test(output_on_failure = True)

    def package(self):
        cmake = self._configure()
        cmake.install()
