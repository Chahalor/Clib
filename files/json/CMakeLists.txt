file(GLOB_RECURSE JSON_SRC CONFIGURE_DEPENDS
	*.c
)
list(FILTER JSON_SRC EXCLUDE REGEX "/controls/")

add_library(clib_files_json OBJECT ${JSON_SRC})

target_link_libraries(clib_files_json PRIVATE clib_includes)

target_include_directories(clib_files_json PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/..
)

clib_add_controls(files_json
	"${CMAKE_CURRENT_SOURCE_DIR}/controls/main.c"
)
