if(APPLE)
  set(PLUGINDIR "/Library/OFX/Plugins")
  set(ARCHDIR "MacOS")
elseif(WIN32)
  set(PLUGINDIR "C:/Program Files (x86)/Common Files/OFX/Plugins")
  set(ARCHDIR "Win64")
elseif(UNIX)
  set(PLUGINDIR "/usr/OFX/Plugins")
  set(ARCHDIR "Linux-x86-64")
else()
  set(PLUGINDIR "/unknown-os")
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
	else()
		set_target_properties(${TARGET} PROPERTIES
                  LINK_FLAGS "-Wl,-fvisibility=hidden,--version-script=${OFX_SUPPORT_SYMBOLS_DIR}/linux.symbols")
	endif()

        # To install plugins: cmake --install Build
        install(TARGETS ${TARGET} DESTINATION "${PLUGINDIR}/${TARGET}.ofx.bundle/Contents/${ARCHDIR}")
        install(FILES ${DIR}/Info.plist DESTINATION "${PLUGINDIR}/${TARGET}.ofx.bundle/Contents")
endfunction()
