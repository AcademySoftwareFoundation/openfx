from conans import ConanFile, CMake

class openfx(ConanFile):
	name = "openfx"
	version = "1.4.0"
	license = "LICENCE"
	url = "https://github.com/ofxa/openfx"
	description = "OpenFX image processing plug-in standard."

	generators = "cmake"
	requires = (
		"opengl/system",
		"expat/2.4.8"
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
	options = {"OFX_EXAMPLE_PLUGINS": [True, False]}
	default_options  = {"OFX_EXAMPLE_PLUGINS": False}

	def build(self):
		cmake = CMake(self)
		cmake.configure()
		cmake.build()

	def package(self):
		self.copy("cmake/*")
		self.copy("LICENSE, README.md, INSTALL.md")
		self.copy("*.h", src="include", dst="include")
		self.copy("*.h", src="HostSupport/include", dst="include")
		self.copy("*.h", src="Support/include", dst="include")
		self.copy("*.h", src="Support/Plugins/include", dst="include")
		self.copy("*.a", dst="lib", keep_path=False)
		self.copy("*.lib", dst="lib", keep_path=False)
		self.copy("*.ofx", dst="bin", keep_path=False)
		self.copy("*.dll", dst="bin", keep_path=False)
		self.copy("*.so", dst="bin", keep_path=False)
		self.copy("*.symbols", dst="symbols", keep_path=False)

	def package_info(self):
		self.cpp_info.libs = ["libOfxHost", "libOfxSupport"]
