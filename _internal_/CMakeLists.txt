file(GLOB_RECURSE INTERNAL_SRC CONFIGURE_DEPENDS
	_config_.c,
	_format_.c,
	_memory_.c,
	_strings_.c
)

add_library(clib_internal OBJECT ${INTERNAL_SRC})

target_link_libraries(clib_internal PRIVATE clib_includes)

target_include_directories(clib_internal PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/..
)
