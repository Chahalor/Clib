file(GLOB_RECURSE DOTENV_SRC
	*.c
)

add_library(clib_interface_dotenv OBJECT ${DOTENV_SRC})

target_link_libraries(clib_interface_dotenv PRIVATE clib_includes)

target_include_directories(clib_interface_dotenv PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/..
)
