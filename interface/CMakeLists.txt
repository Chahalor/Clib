add_subdirectory(log)
add_subdirectory(args)

if (TARGET clib)
	target_sources(clib PRIVATE
		interface.h
		$<TARGET_OBJECTS:clib_interface_log>
		$<TARGET_OBJECTS:clib_interface_args>
	)
endif()
