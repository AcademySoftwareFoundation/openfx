from conans import ConanFile, CMake

class openfx(ConanFile):
	name = "openfx"
	version = "1.4.0"
	license = "BSD-3-Clause"
	url = "https://github.com/AcademySoftwareFoundation/openfx"
	description = "OpenFX image processing plug-in standard"

	generators = "cmake_find_package"
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

	def build(self):
		cmake = CMake(self)
		cmake.configure()
		cmake.build()

	def package(self):
		self.copy("cmake/*")
		self.copy("LICENSE, README.md, INSTALL.md")
		self.copy("*.h", src="include", dst="include")
		self.copy("*.h", src="HostSupport/include", dst="HostSupport/include")
		self.copy("*.h", src="Support/include", dst="Support/include")
		self.copy("*.h", src="Support/Plugins/include", dst="Support/Plugins/include")
		self.copy("*.a", dst="lib", keep_path=False)
		self.copy("*.lib", dst="lib", keep_path=False)
		self.copy("*.ofx", dst="bin", keep_path=False)
		self.copy("*.dll", dst="bin", keep_path=False)
		self.copy("*.so", dst="bin", keep_path=False)
		self.copy("*.symbols", dst="symbols", keep_path=False)

	def package_info(self):
		self.cpp_info.libs = ["libOfxHost", "libOfxSupport"]
