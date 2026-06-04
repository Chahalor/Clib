cmake_minimum_required(VERSION 3.15)

project(clib C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

add_library(clib STATIC)

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
	${CMAKE_CURRENT_SOURCE_DIR}/types
)

option(CLIB_ENABLE_CONTROLS "Enable module control executable and run targets." OFF)
option(CLIB_RUN_CONTROLS_ON_BUILD "Run enabled module controls during the default build." OFF)
set(CLIB_CONTROLS_MODULES "" CACHE STRING
	"Semicolon-separated controls to enable; empty enables every registered module control.")

function(clib_add_controls MODULE_NAME CONTROL_SOURCE)
	cmake_parse_arguments(CLIB_CONTROL "" "" "OBJECTS;LIBRARIES" ${ARGN})

	if (NOT CLIB_ENABLE_CONTROLS)
		return()
	endif()
	if (NOT EXISTS "${CONTROL_SOURCE}")
		return()
	endif()

	if (CLIB_CONTROLS_MODULES)
		list(FIND CLIB_CONTROLS_MODULES "${MODULE_NAME}" _clib_control_index)
		if (_clib_control_index EQUAL -1)
			return()
		endif()
	endif()

	set(_clib_control_exe "clib_controls_${MODULE_NAME}")
	set(_clib_control_run "controls_${MODULE_NAME}")

	add_executable(${_clib_control_exe} EXCLUDE_FROM_ALL
		${CONTROL_SOURCE}
	)
	target_link_libraries(${_clib_control_exe} PRIVATE
		clib_includes
		${CLIB_CONTROL_LIBRARIES}
		$<TARGET_FILE:clib>
	)
	add_dependencies(${_clib_control_exe} clib)
	target_compile_options(${_clib_control_exe} PRIVATE
		$<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Wno-unused-function>
		$<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Wno-error=unused-function>
	)
	foreach (_clib_control_object IN LISTS CLIB_CONTROL_OBJECTS)
		target_sources(${_clib_control_exe} PRIVATE
			$<TARGET_OBJECTS:${_clib_control_object}>
		)
	endforeach()

	add_custom_target(${_clib_control_run}
		COMMAND ${CMAKE_COMMAND} -E echo "Running ${MODULE_NAME} controls"
		COMMAND $<TARGET_FILE:${_clib_control_exe}>
		DEPENDS ${_clib_control_exe}
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		USES_TERMINAL
	)
	set_property(GLOBAL APPEND PROPERTY CLIB_CONTROL_TARGETS
		"${MODULE_NAME}|${_clib_control_exe}"
	)
endfunction()

add_subdirectory(_internal_)
add_subdirectory(standards)
add_subdirectory(files)
add_subdirectory(interface)
add_subdirectory(memory)
add_subdirectory(network)
add_subdirectory(random)
add_subdirectory(times)
add_subdirectory(types)

target_link_libraries(clib PRIVATE
	clib_internal
	clib_standards
	clib_memory
	clib_random
	clib_times
	clib_types
)

target_include_directories(clib PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(clib PRIVATE
	$<TARGET_OBJECTS:clib_memory>
	$<TARGET_OBJECTS:clib_random>
)

target_compile_options(clib
	PUBLIC
		-Wall
		-Wextra
		-Werror
		-g3
		$<$<CONFIG:Debug>:-O0>
		$<$<CONFIG:Debug>:-g3>
		$<$<CONFIG:Release>:-O2>
		$<$<CONFIG:RelWithDebInfo>:-O2>
)

target_compile_definitions(clib
	PUBLIC
		$<$<CONFIG:Debug>:CLIB_DEBUG>
)

clib_add_controls(processor
	"${CMAKE_CURRENT_SOURCE_DIR}/processor/controls/main.c"
)

add_custom_target(controls
	COMMENT "Run all enabled module controls"
)

get_property(_clib_control_targets GLOBAL PROPERTY CLIB_CONTROL_TARGETS)
foreach (_clib_control_target IN LISTS _clib_control_targets)
	string(REPLACE "|" ";" _clib_control_parts "${_clib_control_target}")
	list(GET _clib_control_parts 0 _clib_control_name)
	list(GET _clib_control_parts 1 _clib_control_exe)
	add_dependencies(controls ${_clib_control_exe})
	add_custom_command(TARGET controls POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E echo "Running ${_clib_control_name} controls"
		COMMAND $<TARGET_FILE:${_clib_control_exe}>
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		USES_TERMINAL
	)
endforeach()

if (CLIB_ENABLE_CONTROLS AND CLIB_RUN_CONTROLS_ON_BUILD)
	add_custom_target(clib_controls_all ALL
		DEPENDS controls
	)
endif()
