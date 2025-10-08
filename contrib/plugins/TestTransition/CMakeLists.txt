include(OpenFX)

# Create the plugin target
add_ofx_plugin(TestTransition-support)
target_sources(TestTransition-support PRIVATE testtransition.cpp)
target_link_libraries(TestTransition-support PRIVATE OfxSupport)
target_include_directories(TestTransition-support PRIVATE
    ${CMAKE_SOURCE_DIR}/Support/include
    ${CMAKE_SOURCE_DIR}/include
)