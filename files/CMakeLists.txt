# add_subdirectory(csv)
add_subdirectory(json)
# add_subdirectory(yml)

if (TARGET clib)
	target_sources(clib PRIVATE
		files.h
		# $<TARGET_OBJECTS:clib_files_csv>
		$<TARGET_OBJECTS:clib_files_json>
		# $<TARGET_OBJECTS:clib_files_yml>
	)
endif()
