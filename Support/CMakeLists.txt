set(OFX_SUPPORT_HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(OFX_SUPPORT_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Library")

file(GLOB_RECURSE OFX_SUPPORT_HEADER_FILES "${OFX_SUPPORT_HEADER_DIR}/*.h")
file(GLOB_RECURSE OFX_SUPPORT_LIBRARY_FILES "${OFX_SUPPORT_LIBRARY_DIR}/*.cpp")

# Create OfxSupport static lib
add_library(OfxSupport STATIC ${OFX_HEADER_FILES} ${OFX_SUPPORT_HEADER_FILES} ${OFX_SUPPORT_LIBRARY_FILES})
set_target_properties(OfxSupport PROPERTIES LINKER_LANGUAGE CXX)
if(NOT MSVC)
	set_target_properties(OfxSupport PROPERTIES COMPILE_FLAGS "-fPIC")
endif()
target_include_directories(OfxSupport PUBLIC ${OFX_HEADER_DIR} ${OFX_SUPPORT_HEADER_DIR})

# Install OfxSupport headers and library
install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include"
    DESTINATION "Support"
    FILES_MATCHING
    PATTERN "*.h"
)
install(TARGETS OfxSupport DESTINATION "Support/lib" OPTIONAL)

# Create an alias for all OFX plugins.
add_custom_target(OfxPlugins)

# Build OFX plugins
add_subdirectory(Plugins)
