file(GLOB_RECURSE NETWORK_SRC
	*.c
)

add_library(clib_network OBJECT ${NETWORK_SRC})

target_link_libraries(clib_network PRIVATE clib_includes)

target_include_directories(clib_network PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)
