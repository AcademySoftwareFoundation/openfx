import os

from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.build import can_run
from conan.tools.env import VirtualRunEnv


class openfxTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"

    def requirements(self):
        self.requires(self.tested_reference_str)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.cache_variables["OFX_SUPPORT_SYMBOLS_DIR"] = os.path.join(self.dependencies["openfx"].package_folder,"symbols")
        tc.generate()

        vre = VirtualRunEnv(self)
        vre.environment().define("OFX_PLUGIN_DIR", os.path.join(self.build_folder, self.cpp.build.bindir))
        vre.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def layout(self):
        cmake_layout(self)

    def test(self):
        if can_run(self):
            cmd = os.path.join(self.build_folder, self.cpp.build.bindir, "cacheDemo")
            self.run(cmd, env="conanrun")
