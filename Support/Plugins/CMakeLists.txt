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

	add_ofx_plugin(${PLUGIN})
	target_sources(${PLUGIN} PUBLIC ${PLUGIN_SOURCES})
	target_link_libraries(${PLUGIN} ${CONAN_LIBS} OfxSupport)
	target_include_directories(${PLUGIN} PUBLIC ${OFX_HEADER_DIR} ${OFX_SUPPORT_HEADER_DIR})
endforeach()
