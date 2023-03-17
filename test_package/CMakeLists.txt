cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(openfx CONFIG REQUIRED)

add_executable(cacheDemo ../HostSupport/examples/cacheDemo.cpp)
target_include_directories(cacheDemo PUBLIC ${openfx_INCLUDE_DIRS})
target_link_libraries(cacheDemo openfx::HostSupport)

file(GLOB_RECURSE PLUGIN_SOURCES "../Support/Plugins/Invert/*.cpp")
add_ofx_plugin(invert_plugin ../Support/Plugins/Invert)
target_sources(invert_plugin PUBLIC ${PLUGIN_SOURCES})
target_include_directories(invert_plugin PUBLIC ${openfx_INCLUDE_DIRS})
target_link_libraries(invert_plugin openfx::Support)
