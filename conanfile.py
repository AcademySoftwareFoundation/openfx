from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy, collect_libs
import os.path

required_conan_version = ">=2.0.16"

class openfx(ConanFile):
	name = "openfx"
	version = "1.5.0"
	license = "BSD-3-Clause"
	url = "https://github.com/AcademySoftwareFoundation/openfx"
	description = "OpenFX image processing plug-in standard"
	homepage = "https://openeffects.org/"
	topics = ["graphics", "vfx", "image-processing", "plugins"]

	exports_sources = (
		"cmake/*",
		"Examples/*",
		"HostSupport/*",
		"include/*",
		"scripts/*",
		"Support/*",
		"CMakeLists.txt",
		"LICENSE",
		"README.md",
		"INSTALL.md"
	)

	settings = "os", "arch", "compiler", "build_type"
	options = {"use_opencl": [True, False]}
	default_options = {
		"expat/*:shared": True,
                "use_opencl": False,
                "spdlog/*:header_only": True,
                "fmt/*:header_only": True
	}
	
	def requirements(self):
		if self.options.use_opencl: # for OpenCL examples
			self.requires("opencl-icd-loader/2023.12.14")
			self.requires("opencl-headers/2023.12.14")
		self.requires("opengl/system") # for OpenGL examples
		self.requires("expat/2.7.1") # for HostSupport
		self.requires("cimg/3.3.2") # to draw text into images
		self.requires("spdlog/1.13.0") # for logging

	def layout(self):
		cmake_layout(self)

	def generate(self):
		deps = CMakeDeps(self)
		deps.generate()

		tc = CMakeToolchain(self)
		if self.settings.os == "Windows":
			tc.preprocessor_definitions["WINDOWS"] = 1
			tc.preprocessor_definitions["NOMINMAX"] = 1
		tc.generate()

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

	def package_info(self):
		libs = collect_libs(self)

		self.cpp_info.set_property("cmake_build_modules", [os.path.join("cmake", "OpenFX.cmake")])
		self.cpp_info.components["Api"].includedirs = ["include"]
		self.cpp_info.components["HostSupport"].libs = [i for i in libs if "OfxHost" in i]
		self.cpp_info.components["HostSupport"].includedirs = ["HostSupport/include"]
		self.cpp_info.components["HostSupport"].requires = ["expat::expat"]
		self.cpp_info.components["Support"].libs = [i for i in libs if "OfxSupport" in i]
		self.cpp_info.components["Support"].includedirs = ["Support/include"]
		self.cpp_info.components["Support"].requires = ["opengl::opengl", "cimg::cimg", "spdlog::spdlog"]

		if self.settings.os == "Windows":
			win_defines = ["WINDOWS", "NOMINMAX"]
			self.cpp_info.components["Api"].defines = win_defines
			self.cpp_info.components["HostSupport"].defines = win_defines
			self.cpp_info.components["Support"].defines = win_defines
