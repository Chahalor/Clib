add_subdirectory(log)

if (TARGET clib)
	target_sources(clib PRIVATE
		# interface.h
		$<TARGET_OBJECTS:clib_interface_log>
	)
endif()
