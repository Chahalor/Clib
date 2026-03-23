file(GLOB_RECURSE PROTOCOLS_SRC CONFIGURE_DEPENDS
	**/*.c
)

add_library(clib_files_protocols OBJECT ${PROTOCOLS_SRC})

target_link_libraries(clib_files_protocols PRIVATE clib_includes)

target_include_directories(clib_files_protocols PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/..
)
