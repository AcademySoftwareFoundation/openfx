from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy
import os.path

required_conan_version = ">=1.59.0"

class openfx(ConanFile):
	name = "openfx"
	version = "1.4.0"
	license = "BSD-3-Clause"
	url = "https://github.com/AcademySoftwareFoundation/openfx"
	description = "OpenFX image processing plug-in standard"
	
	generators = "CMakeToolchain", "CMakeDeps"
	requires = (
		"opengl/system",
		"expat/2.4.8"   # for HostSupport
	)

	exports_sources = (
		"cmake/*",
		"Examples/*",
		"HostSupport/*",
		"include/*",
		"scripts/*",
		"Support/*",
		"symbols/*",
		"CMakeLists.txt",
		"LICENSE",
		"README.md",
		"INSTALL.md"
	)

	settings = "os", "arch", "compiler", "build_type"

	
	def layout(self):
		cmake_layout(self)

	def build(self):
		cmake = CMake(self)
		cmake.configure()
		cmake.build()

	def package(self):
		copy(self, "cmake/*", src=self.source_folder, dst=self.package_folder)
		copy(self, "LICENSE, README.md, INSTALL.md", src=self.source_folder, dst=self.package_folder)
		copy(self, "include/*.h", src=self.source_folder, dst=self.package_folder)
		copy(self,"HostSupport/include/*.h", src=self.source_folder, dst=self.package_folder)
		copy(self,"Support/*.h", src=self.source_folder, dst=self.package_folder)
		copy(self,"Support/Plugins/include/*.h", src=self.source_folder, dst=self.package_folder)
		copy(self,"*.a", src=self.build_folder, dst=os.path.join(self.package_folder, "lib"), keep_path=False)
		copy(self,"*.lib", src=self.build_folder, dst=os.path.join(self.package_folder, "lib"), keep_path=False)
		copy(self,"*.ofx", src=self.build_folder, dst=os.path.join(self.package_folder, "bin"), keep_path=False)
		copy(self,"*.dll", src=self.build_folder, dst=os.path.join(self.package_folder, "bin"), keep_path=False)
		copy(self,"*.so", src=self.build_folder, dst=os.path.join(self.package_folder, "bin"), keep_path=False)
		copy(self,"*.symbols", src=self.source_folder, dst=os.path.join(self.package_folder, "symbols"), keep_path=False)

	def package_info(self):
		self.cpp_info.libs = ["libOfxHost", "libOfxSupport"]
