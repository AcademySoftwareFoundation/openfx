if(NOT PLUGIN_INSTALLDIR)
  if(APPLE)
    set(PLUGIN_INSTALLDIR "/Library/OFX/Plugins")
  elseif(WIN32)
    set(PLUGIN_INSTALLDIR "C:/Program Files/Common Files/OFX/Plugins")
  elseif(UNIX)
    set(PLUGIN_INSTALLDIR "/usr/OFX/Plugins")
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

# Add a new OFX plugin target
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

  if(NOT DEFINED OFX_SUPPORT_SYMBOLS_DIR)
    if (NOT DEFINED CONAN_OPENFX_ROOT)
      message(FATAL_ERROR "Define OFX_SUPPORT_SYMBOLS_DIR to use add_ofx_plugin().")
    endif()
    set(OFX_SUPPORT_SYMBOLS_DIR ${CONAN_OPENFX_ROOT}/symbols)
  endif()

  # Add extra flags to the link step of the plugin
  if(APPLE)
    set_target_properties(${TARGET} PROPERTIES
                          LINK_FLAGS "-fvisibility=hidden -exported_symbols_list,${OFX_SUPPORT_SYMBOLS_DIR}/osx.symbols")
  elseif(WIN32)
    if (MSVC)
            set_target_properties(${TARGET} PROPERTIES
                                  LINK_FLAGS "/def:${OFX_SUPPORT_SYMBOLS_DIR}/windows.symbols")
    elseif(MINGW OR MSYS)
      set_target_properties(${TARGET} PROPERTIES
                            LINK_FLAGS "-Wl,-fvisibility=hidden,--version-script=${OFX_SUPPORT_SYMBOLS_DIR}/mingw.symbols")
    endif()
  else()                        # Linux
    set_target_properties(${TARGET} PROPERTIES
                          LINK_FLAGS "-Wl,-fvisibility=hidden,--version-script=${OFX_SUPPORT_SYMBOLS_DIR}/linux.symbols")
  endif()

  # To install plugins: cmake --install Build
  install(TARGETS ${TARGET}
    RUNTIME DESTINATION "${PLUGIN_INSTALLDIR}/${TARGET}.ofx.bundle/Contents/${ARCHDIR}"
    LIBRARY DESTINATION "${PLUGIN_INSTALLDIR}/${TARGET}.ofx.bundle/Contents/${ARCHDIR}"
    BUNDLE DESTINATION "${PLUGIN_INSTALLDIR}/${TARGET}.ofx.bundle/Contents/${ARCHDIR}"
)

  # Use info.plist in DIR or else current dir
  file(REAL_PATH ${DIR}/Info.plist INFO_PLIST)
  if(EXISTS ${INFO_PLIST})
    install(FILES ${INFO_PLIST} DESTINATION "${PLUGIN_INSTALLDIR}/${TARGET}.ofx.bundle/Contents")
  else()
    file(REAL_PATH ./Info.plist INFO_PLIST)
    if(EXISTS ${INFO_PLIST})
      install(FILES ${INFO_PLIST} DESTINATION "${PLUGIN_INSTALLDIR}/${TARGET}.ofx.bundle/Contents")
    endif()
  endif()

endfunction()
