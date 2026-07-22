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
	options = {
		"build_examples": [True, False],
		"use_opencl": [True, False],
	}
	default_options = {
		"expat/*:shared": True,
		"build_examples": False,
		"use_opencl": False,
		"spdlog/*:header_only": True,
		"fmt/*:header_only": True
	}

	def requirements(self):
		self.requires("expat/2.7.1") # for HostSupport
		# Everything below is used only by the example plugins.
		if self.options.build_examples:
			self.requires("opengl/system")
			self.requires("cimg/3.3.2") # to draw text into images
			self.requires("spdlog/1.13.0") # for logging
			if self.options.use_opencl:
				self.requires("opencl-icd-loader/2023.12.14")
				self.requires("opencl-headers/2023.12.14")

	def layout(self):
		cmake_layout(self)

	def generate(self):
		deps = CMakeDeps(self)
		deps.generate()

		tc = CMakeToolchain(self)
		tc.cache_variables["BUILD_EXAMPLE_PLUGINS"] = bool(self.options.build_examples)
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
		self.cpp_info.components["HostSupport"].requires = ["Api", "expat::expat"]
		self.cpp_info.components["Support"].libs = [i for i in libs if "OfxSupport" in i]
		self.cpp_info.components["Support"].includedirs = ["include/Support", "include/Support/Plugins"]
		self.cpp_info.components["Support"].requires = ["Api"]
		if self.options.build_examples:
			# The packaged example plugins use these; consumers of the
			# libraries alone (build_examples=False) need only expat.
			self.cpp_info.components["Support"].requires += ["opengl::opengl", "cimg::cimg"]
			self.cpp_info.components["HostSupport"].requires += ["spdlog::spdlog"]
			if self.options.use_opencl:
				self.cpp_info.components["Support"].requires += [
					"opencl-icd-loader::opencl-icd-loader", "opencl-headers::opencl-headers"]
