cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(openfx CONFIG REQUIRED)

add_executable(test_package src/test_package.cpp)
target_include_directories(test_package PUBLIC ${openfx_INCLUDE_DIRS})
target_link_libraries(test_package openfx::HostSupport)

file(GLOB_RECURSE PLUGIN_SOURCES "../Support/Plugins/Invert/*.cpp")
add_ofx_plugin(invert_plugin ../Support/Plugins/Invert)
target_sources(invert_plugin PUBLIC ${PLUGIN_SOURCES})
target_include_directories(invert_plugin PUBLIC ${openfx_INCLUDE_DIRS})
target_link_libraries(invert_plugin openfx::Support)