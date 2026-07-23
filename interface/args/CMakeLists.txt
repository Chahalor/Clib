#[[
@file CMakeLists.txt
@brief CMake rules for the `interface/args` module.
]]

file(GLOB_RECURSE ARGS_SRC CONFIGURE_DEPENDS
	*.c
)
list(FILTER ARGS_SRC EXCLUDE REGEX "/controls/")

add_library(clib_interface_args OBJECT ${ARGS_SRC})

target_link_libraries(clib_interface_args PRIVATE clib_includes)

target_include_directories(clib_interface_args PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/..
)

clib_add_controls(interface_args
	"${CMAKE_CURRENT_SOURCE_DIR}/controls/main.c"
)
