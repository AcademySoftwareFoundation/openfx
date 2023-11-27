set(OFX_SUPPORT_HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Support/include")
file(GLOB_RECURSE OFX_SUPPORT_HEADER_FILES "${OFX_SUPPORT_HEADER_DIR}/*.h")

set(PLUGINS
	Basic
	Custom
	DepthConverter
	Invert
	OpenGL
	Rectangle
	Test
	)

set(GPU_PLUGINS
        GPUGain
        )

foreach(PLUGIN IN LISTS PLUGINS)
	file(GLOB_RECURSE PLUGIN_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}/*.cpp")

        set(TGT example-${PLUGIN})
	add_ofx_plugin(${TGT} ${PLUGIN})
	target_sources(${TGT} PUBLIC ${PLUGIN_SOURCES})
	target_link_libraries(${TGT} ${CONAN_LIBS})
	target_include_directories(${TGT} PUBLIC ${OFX_HEADER_DIR} ${OFX_SUPPORT_HEADER_DIR})
endforeach()

if (OFX_SUPPORTS_OPENGLRENDER)
    foreach(PLUGIN IN LISTS GPU_PLUGINS)
	file(GLOB_RECURSE PLUGIN_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}/*.cpp")
	file(GLOB_RECURSE PLUGIN_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}/*.mm")

        set(TGT example-${PLUGIN})
	add_ofx_plugin(${TGT} ${PLUGIN})
	target_sources(${TGT} PUBLIC ${PLUGIN_SOURCES})
	target_include_directories(${TGT} PUBLIC ${OFX_HEADER_DIR} ${OFX_SUPPORT_HEADER_DIR})
	target_link_libraries(${TGT} ${CONAN_LIBS})
        target_link_libraries(${TGT} "-framework Metal" "-framework Foundation" "-framework QuartzCore")
    endforeach()
endif()

target_link_libraries(example-OpenGL PRIVATE opengl::opengl)
target_link_libraries(example-Custom PRIVATE opengl::opengl)
