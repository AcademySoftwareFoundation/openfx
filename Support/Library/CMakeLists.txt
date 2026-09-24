set(OFX_SUPPORT_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
file(GLOB_RECURSE OFX_SUPPORT_LIBRARY_FILES "${OFX_SUPPORT_LIBRARY_DIR}/*.cpp")

add_library(OfxSupport STATIC
	${OFX_HEADER_FILES}
	${OFX_SUPPORT_HEADER_FILES}
	${OFX_SUPPORT_LIBRARY_FILES})

target_compile_features(OfxSupport PUBLIC cxx_std_17)
set_target_properties(OfxSupport PROPERTIES LINKER_LANGUAGE CXX)
if(NOT MSVC)
	set_target_properties(OfxSupport PROPERTIES COMPILE_FLAGS "-fPIC")
endif()

# Plugins link this library in and should export only the OFX entry points,
# which ofxCore.h declares with OfxExport, so hide everything else.
set_target_properties(OfxSupport PROPERTIES CXX_VISIBILITY_PRESET hidden
                                            VISIBILITY_INLINES_HIDDEN ON)

target_include_directories(OfxSupport PUBLIC
	${OFX_HEADER_DIR}
	${OFX_SUPPORT_HEADER_DIR})
