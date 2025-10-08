include(OpenFX)

# Create the plugin target
add_ofx_plugin(TestGenerator-support)
target_sources(TestGenerator-support PRIVATE testgenerator.cpp)
target_link_libraries(TestGenerator-support PRIVATE OfxSupport)
target_include_directories(TestGenerator-support PRIVATE
    ${CMAKE_SOURCE_DIR}/Support/include
    ${CMAKE_SOURCE_DIR}/include
)