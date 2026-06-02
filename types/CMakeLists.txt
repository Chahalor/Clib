file(GLOB_RECURSE TYPES_SRC
	*.c
)

add_library(clib_types OBJECT ${TYPES_SRC})

target_link_libraries(clib_types PRIVATE clib_includes)

target_include_directories(clib_types PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)
