file(GLOB_RECURSE JSON_SRC
	*.c
)

add_library(clib_interface_json OBJECT ${JSON_SRC})

target_link_libraries(clib_interface_json PRIVATE clib_includes)

target_include_directories(clib_interface_json PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/..
)
