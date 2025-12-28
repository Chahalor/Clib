add_library(clib_times OBJECT
	convert.c
)

target_link_libraries(clib_times PRIVATE clib_includes)

target_include_directories(clib_times PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)
