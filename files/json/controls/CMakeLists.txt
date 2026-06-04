clib_add_controls(files_json
	"${CMAKE_CURRENT_LIST_DIR}/main.c"
)

if (TARGET clib_controls_files_json AND BUILD_TESTING)
	add_test(NAME controls_files_json
		COMMAND clib_controls_files_json
	)
	set_tests_properties(controls_files_json PROPERTIES
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
	)
endif()
