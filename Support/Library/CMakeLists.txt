set(OFX_SUPPORT_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
file(GLOB_RECURSE OFX_SUPPORT_LIBRARY_FILES "${OFX_SUPPORT_LIBRARY_DIR}/*.cpp")

add_library(OfxSupport STATIC
	${OFX_HEADER_FILES}
	${OFX_SUPPORT_HEADER_FILES}
	${OFX_SUPPORT_LIBRARY_FILES})

set_target_properties(OfxSupport PROPERTIES LINKER_LANGUAGE CXX)
if(NOT MSVC)
	set_target_properties(OfxSupport PROPERTIES COMPILE_FLAGS "-fPIC")
endif()

target_include_directories(OfxSupport PUBLIC
	${OFX_HEADER_DIR}
	${OFX_SUPPORT_HEADER_DIR})
