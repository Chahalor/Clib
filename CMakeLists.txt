cmake_minimum_required(VERSION 3.15)

project(clib C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(clib STATIC)

add_subdirectory(standards)
add_subdirectory(interface)
add_subdirectory(memory)
add_subdirectory(network)
add_subdirectory(random)

target_link_libraries(clib PRIVATE
	clib_standards
	clib_interface
	clib_memory
	clib_network
	clib_random
)

target_include_directories(clib PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(clib PRIVATE
	$<TARGET_OBJECTS:clib_interface>
	$<TARGET_OBJECTS:clib_memory>
	$<TARGET_OBJECTS:clib_network>
	$<TARGET_OBJECTS:clib_random>
)
