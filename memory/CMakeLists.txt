file(GLOB_RECURSE MEMORY_SRC
	*.c
)
list(FILTER MEMORY_SRC EXCLUDE REGEX "/controls/")

add_library(clib_memory OBJECT ${MEMORY_SRC})

target_link_libraries(clib_memory PRIVATE clib_includes)

target_include_directories(clib_memory PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)
