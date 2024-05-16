set(OFX_SUPPORT_HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Support/include")

set(PLUGINS
	Basic
	ChoiceParams
	ColourSpace
	Custom
	DepthConverter
	Invert
	OpenGL
	Rectangle
	Test
        DrawSuite
	)

foreach(PLUGIN IN LISTS PLUGINS)
	file(GLOB_RECURSE PLUGIN_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}/*.cpp")

        set(TGT example-${PLUGIN})
	add_ofx_plugin(${TGT} ${PLUGIN})
	target_sources(${TGT} PUBLIC ${PLUGIN_SOURCES})
	target_link_libraries(${TGT} ${CONAN_LIBS})
	target_include_directories(${TGT} PUBLIC ${OFX_HEADER_DIR} ${OFX_SUPPORT_HEADER_DIR})
        if (OFX_SUPPORTS_OPENCLRENDER)
          target_link_libraries(${TGT} PRIVATE OpenCL::Headers OpenCL::OpenCL)
        endif()
endforeach()

target_link_libraries(example-OpenGL PRIVATE opengl::opengl)
target_link_libraries(example-Custom PRIVATE opengl::opengl)
target_link_libraries(example-ColourSpace PRIVATE cimg::cimg)
target_link_libraries(example-ColourSpace PRIVATE spdlog::spdlog_header_only)
