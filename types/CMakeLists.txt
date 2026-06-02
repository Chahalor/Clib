file(GLOB_RECURSE TYPES_SRC
	*.c
)
list(FILTER TYPES_SRC EXCLUDE REGEX "/controls/")

add_library(clib_types OBJECT ${TYPES_SRC})

target_link_libraries(clib_types PRIVATE clib_includes)

target_include_directories(clib_types
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
)

clib_add_controls(types
	"${CMAKE_CURRENT_SOURCE_DIR}/controls/main.c"
	OBJECTS clib_types
)
