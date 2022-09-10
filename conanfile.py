from conans import ConanFile, CMake

class DeltaCRDT(ConanFile):
    name = "delta_crdt"
    version = "0.0.1"
    license = "Apache 2.0/MIT"
    author = "Maxim Eryomenko <moeryomenko@gmail.com>"
    url = "https://github.com/moeryomenko/delta_crdt"
    description = "Implementation δ-based Conflict-free Replicated Data Types on C++20"
    topics = ("δ-CRDT", "CRDT", "Replication")

    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake_find_package"

    options = {
        "build_tests": [True, False],
    }
    default_options = {
        "build_tests": False,
    }

    exports_sources = "*"

    _cmake = False

    def requirements(self):
        if self.options.build_tests:
            self.requires("boost-ext-ut/1.1.8")

    def build(self):
        if self.options.build_tests:
            cmake = self._configure_cmake()
            cmake.build()
            cmake.test()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

    def _configure_cmake(self):
        if self._cmake:
            return self._cmake

        self._cmake = CMake(self)
        self._cmake.definitions["CRDT_BUILD_TESTS"] = self.options.build_tests
        self._cmake.configure()
        return self._cmake
