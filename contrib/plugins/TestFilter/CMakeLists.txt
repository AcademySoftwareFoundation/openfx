include(OpenFX)

# Create the plugin target
add_ofx_plugin(TestFilter-support)
target_sources(TestFilter-support PRIVATE testfilter.cpp)
target_link_libraries(TestFilter-support PRIVATE OfxSupport)
target_include_directories(TestFilter-support PRIVATE
    ${CMAKE_SOURCE_DIR}/Support/include
    ${CMAKE_SOURCE_DIR}/include
)