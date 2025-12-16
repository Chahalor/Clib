add_library(clib_interface OBJECT)

add_subdirectory(io)
add_subdirectory(json)
add_subdirectory(log)

target_sources(clib_interface PRIVATE
	interface.h
)

target_link_libraries(clib_interface PRIVATE
	clib_interface_io
	clib_interface_json
	clib_interface_log
)
