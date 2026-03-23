add_subdirectory(communication)
add_subdirectory(protocols)

if (TARGET clib)
	target_sources(clib PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/../network.h
		$<TARGET_OBJECTS:clib_comm>
		$<TARGET_OBJECTS:clib_files_protocols>
	)
endif()
