file(GLOB_RECURSE LOG_SRC
	*.c
)

add_library(clib_interface_log OBJECT ${LOG_SRC})

target_link_libraries(clib_interface_log PRIVATE clib_includes)

target_include_directories(clib_interface_log PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/..
)
