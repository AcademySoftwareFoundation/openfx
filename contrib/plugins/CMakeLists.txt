# Contrib Plugins - Automatic Discovery
# This file automatically includes all plugin directories in contrib/plugins/

# Get all subdirectories that contain CMakeLists.txt files
file(GLOB_RECURSE plugin_cmake_files "*/CMakeLists.txt")

foreach(cmake_file ${plugin_cmake_files})
    # Get the directory containing the CMakeLists.txt
    get_filename_component(plugin_dir "${cmake_file}" DIRECTORY)

    # Get the relative path from contrib/plugins to the plugin directory
    file(RELATIVE_PATH relative_plugin_dir "${CMAKE_CURRENT_SOURCE_DIR}" "${plugin_dir}")

    # Only include direct subdirectories (avoid nested builds)
    if(NOT "${relative_plugin_dir}" MATCHES "/")
        message(STATUS "Including contrib plugin: ${relative_plugin_dir}")
        add_subdirectory("${relative_plugin_dir}")
    endif()
endforeach()