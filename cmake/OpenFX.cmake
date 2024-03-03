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

set(INSTALLDIR "${PLUGINDIR}" CACHE PATH "Path to install plugins")
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

	# Set symbol visibility hidden. Individual symbols are exposed via
	# __declspec(dllexport) or __attribute__((visibility("default")))
	set_target_properties(${TARGET} PROPERTIES
	  C_VISIBILITY_PRESET hidden
	  CXX_VISIBILITY_PRESET hidden)

	# To install plugins: cmake --install build/Release
	install(TARGETS ${TARGET} DESTINATION "${INSTALLDIR}/${TARGET}.ofx.bundle/Contents/${ARCHDIR}")
	install(FILES ${DIR}/Info.plist DESTINATION "${INSTALLDIR}/${TARGET}.ofx.bundle/Contents")
endfunction()
