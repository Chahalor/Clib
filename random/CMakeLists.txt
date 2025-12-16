add_library(clib_random OBJECT
	random.c
)

target_link_libraries(clib_random PRIVATE clib_includes)

target_include_directories(clib_random PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)
