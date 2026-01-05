file(GLOB_RECURSE JSON_SRC CONFIGURE_DEPENDS
	*.c
)

add_library(clib_files_json OBJECT ${JSON_SRC})

target_link_libraries(clib_files_json PRIVATE clib_includes)

target_include_directories(clib_files_json PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/..
)
