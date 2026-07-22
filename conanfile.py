from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy, collect_libs
import os.path

required_conan_version = ">=1.59.0"

class openfx(ConanFile):
	name = "openfx"
	version = "1.4.0"
	license = "BSD-3-Clause"
	url = "https://github.com/AcademySoftwareFoundation/openfx"
	description = "OpenFX image processing plug-in standard"
	
	exports_sources = (
		"cmake/*",
		"Examples/*",
		"HostSupport/*",
		"include/*",
		"scripts/*",
		"Support/*",
		"CMakeLists.txt",
		"LICENSE*",
		"README.md",
		"install.md"
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
		tc.generate()

	def build(self):
		cmake = CMake(self)
		cmake.configure()
		cmake.build()

	def package(self):
		src = self.source_folder
		pkg = self.package_folder
		inc = os.path.join(pkg, "include")

		# Headers, all under include/ following the standard layout. The OFX C
		# API headers sit at the root; the host- and plugin-support headers go
		# into namespaced subdirectories.
		copy(self, "*.h", src=os.path.join(src, "include"), dst=inc)
		copy(self, "*.h", src=os.path.join(src, "HostSupport", "include"),
		     dst=os.path.join(inc, "HostSupport"))
		copy(self, "*.h", src=os.path.join(src, "Support", "include"),
		     dst=os.path.join(inc, "Support"))
		# Plugin support helper headers use a .H extension; match both cases.
		copy(self, "*.[hH]", src=os.path.join(src, "Support", "Plugins", "include"),
		     dst=os.path.join(inc, "Support", "Plugins"))

		# The CMake build module (add_ofx_plugin) goes in lib/cmake, alongside
		# the Info.plist template it needs, so it resolves relative to itself
		# when consumed from the package (see cmake/OpenFX.cmake).
		cmake_dst = os.path.join(pkg, "lib", "cmake")
		copy(self, "OpenFX.cmake", src=os.path.join(src, "cmake"), dst=cmake_dst)
		copy(self, "Info.plist.in", src=os.path.join(src, "Examples"), dst=cmake_dst)

		# License in a per-package subdir so installing many packages into one
		# prefix (as aswf-docker does) doesn't clash; other docs under res/.
		copy(self, "LICENSE*", src=src, dst=os.path.join(pkg, "licenses", "openfx"))
		copy(self, "README.md", src=src, dst=os.path.join(pkg, "res", "openfx"))
		copy(self, "install.md", src=src, dst=os.path.join(pkg, "res", "openfx"))

		# Static libraries, then any built example plugins.
		lib = os.path.join(pkg, "lib")
		bin = os.path.join(pkg, "bin")
		copy(self, "*.a", src=self.build_folder, dst=lib, keep_path=False)
		copy(self, "*.lib", src=self.build_folder, dst=lib, keep_path=False)
		copy(self, "*.ofx", src=self.build_folder, dst=bin, keep_path=False)
		copy(self, "*.dll", src=self.build_folder, dst=bin, keep_path=False)
		copy(self, "*.so", src=self.build_folder, dst=bin, keep_path=False)

	def package_info(self):
		libs = collect_libs(self)

		self.cpp_info.set_property("cmake_build_modules", [os.path.join("lib", "cmake", "OpenFX.cmake")])
		self.cpp_info.components["Api"].includedirs = ["include"]
		self.cpp_info.components["HostSupport"].libs = [i for i in libs if "OfxHost" in i]
		self.cpp_info.components["HostSupport"].includedirs = ["include/HostSupport"]
		# spdlog is used by the example/host logging helpers (header-only here).
		self.cpp_info.components["HostSupport"].requires = ["expat::expat", "spdlog::spdlog"]
		self.cpp_info.components["Support"].libs = [i for i in libs if "OfxSupport" in i]
		self.cpp_info.components["Support"].includedirs = ["include/Support", "include/Support/Plugins"]
		# cimg is used by the support example plugins to draw text (header-only).
		self.cpp_info.components["Support"].requires = ["opengl::opengl", "cimg::cimg"]
