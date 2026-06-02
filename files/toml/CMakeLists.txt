file(GLOB_RECURSE TOML_SRC CONFIGURE_DEPENDS
	*.c
)
list(FILTER TOML_SRC EXCLUDE REGEX "/controls/")

add_library(clib_files_toml OBJECT ${TOML_SRC})

target_link_libraries(clib_files_toml PRIVATE clib_includes)

target_include_directories(clib_files_toml PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/..
)

clib_add_controls(files_toml
	"${CMAKE_CURRENT_SOURCE_DIR}/controls/main.c"
)
