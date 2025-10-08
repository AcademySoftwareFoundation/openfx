include(OpenFX)

# Create the plugin target
add_ofx_plugin(MyFirstBrightness-support)
target_sources(MyFirstBrightness-support PRIVATE brightness.cpp)
target_link_libraries(MyFirstBrightness-support PRIVATE OfxSupport)
target_include_directories(MyFirstBrightness-support PRIVATE
    ${CMAKE_SOURCE_DIR}/Support/include
    ${CMAKE_SOURCE_DIR}/include
)