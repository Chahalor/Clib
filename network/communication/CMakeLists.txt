file(GLOB_RECURSE COMM_SRC
	*.c
)

add_library(clib_comm OBJECT ${COMM_SRC})

target_link_libraries(clib_comm PRIVATE clib_includes)

target_include_directories(clib_comm PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)
