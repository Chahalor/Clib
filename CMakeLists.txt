cmake_minimum_required(VERSION 3.15)

project(clib C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

add_library(clib STATIC)

add_subdirectory(_internal_)
add_subdirectory(standards)
add_subdirectory(files)
add_subdirectory(interface)
add_subdirectory(memory)
add_subdirectory(network)
add_subdirectory(random)
add_subdirectory(times)

target_link_libraries(clib PRIVATE
	clib_internal
	clib_standards
	clib_interface
	clib_memory
	clib_network
	clib_random
	clib_times
)

target_include_directories(clib PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(clib PRIVATE
	$<TARGET_OBJECTS:clib_memory>
	$<TARGET_OBJECTS:clib_network>
	$<TARGET_OBJECTS:clib_random>
)

add_library(clib_includes INTERFACE)

target_include_directories(clib_includes INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/_internal_
	${CMAKE_CURRENT_SOURCE_DIR}/standards
	${CMAKE_CURRENT_SOURCE_DIR}/files
	${CMAKE_CURRENT_SOURCE_DIR}/interface
	${CMAKE_CURRENT_SOURCE_DIR}/memory
	${CMAKE_CURRENT_SOURCE_DIR}/network
	${CMAKE_CURRENT_SOURCE_DIR}/random
	${CMAKE_CURRENT_SOURCE_DIR}/times
)

target_compile_options(clib
	PUBLIC
		-Wall
		-Wextra
		-Werror
		$<$<CONFIG:Debug>:-O0>
		$<$<CONFIG:Debug>:-g3>
		$<$<CONFIG:Release>:-O2>
		$<$<CONFIG:RelWithDebInfo>:-O2>
)

target_compile_definitions(clib
	PUBLIC
		$<$<CONFIG:Debug>:CLIB_DEBUG>
)
