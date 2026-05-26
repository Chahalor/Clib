# add_subdirectory(csv)
add_subdirectory(json)
add_subdirectory(toml)
# add_subdirectory(yml)

if (TARGET clib)
	target_sources(clib PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/../files.h
		$<TARGET_OBJECTS:clib_files_json>
		$<TARGET_OBJECTS:clib_files_toml>
	)
endif()
