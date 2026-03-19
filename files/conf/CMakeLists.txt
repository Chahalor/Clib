file(GLOB_RECURSE CONF_SRC CONFIGURE_DEPENDS
	*.c
)

add_library(clib_files_conf OBJECT ${CONF_SRC})

target_link_libraries(clib_files_conf PRIVATE clib_includes)

target_include_directories(clib_files_conf PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/..
)
