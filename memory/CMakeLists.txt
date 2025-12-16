file(GLOB_RECURSE MEMORY_SRC
	*.c
)

add_library(clib_memory OBJECT ${MEMORY_SRC})

target_include_directories(clib_memory PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)
