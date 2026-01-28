add_subdirectory(log)
add_subdirectory(args)
add_subdirectory(dotenv)

if (TARGET clib)
	target_sources(clib PRIVATE
		$<TARGET_OBJECTS:clib_interface_log>
		$<TARGET_OBJECTS:clib_interface_args>
		$<TARGET_OBJECTS:clib_interface_dotenv>
	)
endif()
