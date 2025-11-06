set(OFX_SUPPORT_HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(OFX_CPP_BINDINGS_HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../openfx-cpp/include")
file(GLOB_RECURSE OFX_SUPPORT_HEADER_FILES "${OFX_SUPPORT_HEADER_DIR}/*.h")

add_subdirectory(Library)
if(BUILD_EXAMPLE_PLUGINS)
  add_subdirectory(Plugins)
  add_subdirectory(PropTester)
endif()
