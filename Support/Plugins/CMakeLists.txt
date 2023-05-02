include(OpenFX)

set(PLUGINS
	Basic
	Field
	Generator
	Invert
	MultiBundle
	Retimer
	Tester
	Transition)

foreach(PLUGIN IN LISTS PLUGINS)
	file(GLOB_RECURSE PLUGIN_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}/*.cpp")

        set(TGT example-${PLUGIN}-support)
	add_ofx_plugin(${TGT} ${PLUGIN})
	target_sources(${TGT} PUBLIC ${PLUGIN_SOURCES})
	target_link_libraries(${TGT} ${CONAN_LIBS} OfxSupport opengl::opengl)
	target_include_directories(${TGT} PUBLIC ${OFX_HEADER_DIR} ${OFX_SUPPORT_HEADER_DIR})
endforeach()
