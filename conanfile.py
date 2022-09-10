from conans import ConanFile, CMake

class DeltaCRDT(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"

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
        cmake = self._configure_cmake()
        cmake.build()
        if self.options.build_tests:
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
