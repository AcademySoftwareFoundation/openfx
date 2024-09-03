# SPDX-License-Identifier: BSD-3-Clause

if(NOT PLUGIN_INSTALLDIR)
  if(APPLE)
    set(PLUGIN_INSTALLDIR "/Library/OFX/Plugins/OpenFX Examples")
  elseif(WIN32)
    set(PLUGIN_INSTALLDIR "C:/Program Files/Common Files/OFX/Plugins/OpenFX Examples")
  elseif(UNIX)
    set(PLUGIN_INSTALLDIR "/usr/OFX/Plugins/OpenFX Examples")
  else()
    set(PLUGIN_INSTALLDIR "/unknown-os")
  endif()
endif()

if(APPLE)
  set(ARCHDIR "MacOS")
elseif(WIN32)
  set(ARCHDIR "Win64")
elseif(UNIX)
  set(ARCHDIR "Linux-x86-64")
else()
  set(ARCHDIR "unknown-arch")
endif()

# Add a new OFX plugin "target".
#
# Arguments: TARGET
# Optional argument: DIR, defaults to same as TARGET (use when renaming TARGET)
function(add_ofx_plugin TARGET)
  if(${ARGC} GREATER 1)
    set(DIR ${ARGN})
  else()
    set(DIR ${TARGET})
  endif()
  if(APPLE)
    add_library(${TARGET} MODULE) # build as an OSX bundle
  else()
    add_library(${TARGET} SHARED) # build as shared lib/DLL
  endif()
  set_target_properties(${TARGET} PROPERTIES SUFFIX ".ofx" PREFIX "")

  set(PLUGIN_NAME ${TARGET})
  set(PLUGIN_EXE ${TARGET}.ofx)
  set(BUNDLE_IDENTIFIER org.openeffects.plugins.${TARGET})
  set(BUNDLE_SIGNATURE "????")  # any value is OK here, ignored by modern MacOS
  set(PLUGIN_VERSION "1.0.0")

  configure_file(${CMAKE_SOURCE_DIR}/Examples/Info.plist.in
                 "${PLUGIN_INSTALLDIR}/${TARGET}.ofx.bundle/Contents/Info.plist")

  # Set symbol visibility hidden. Individual symbols are exposed via
  # __declspec(dllexport) or __attribute__((visibility("default")))
  set_target_properties(${TARGET} PROPERTIES C_VISIBILITY_PRESET hidden
                                             CXX_VISIBILITY_PRESET hidden)

  # To install plugins: cmake --install Build
  install(
    TARGETS ${TARGET}
    DESTINATION "${PLUGIN_INSTALLDIR}/${TARGET}.ofx.bundle/Contents/${ARCHDIR}")
endfunction()
