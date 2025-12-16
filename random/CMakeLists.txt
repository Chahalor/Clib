add_library(clib_random OBJECT
	random.c
)

target_include_directories(clib_random PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)
