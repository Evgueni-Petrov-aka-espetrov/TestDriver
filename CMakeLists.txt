cmake_minimum_required(VERSION 3.5)

project(TestDriver C)

enable_testing()

find_library(MATH_LIBRARY m)

function(SET_COMPILER_SETTINGS target)
	cmake_parse_arguments(
		SET_COMPILER_SETTINGS
		""
		"EXTENSIONS"
		""
		${ARGN}
	)

	set_target_properties(${target} PROPERTIES
		C_STANDARD 11
		C_STANDARD_REQUIRED YES
		C_EXTENSIONS ${SET_COMPILER_SETTINGS_EXTENSIONS}
	)

	target_compile_options(${target} PRIVATE
		$<$<OR:$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>,$<C_COMPILER_ID:GNU>>:
		-Wall
		-Wextra
		-Wno-missing-field-initializers
		-pedantic
		-pedantic-errors
		>
		$<$<OR:$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>>:
		-Weverything
		-Wno-padded
		-Wno-disabled-macro-expansion
		>
		$<$<C_COMPILER_ID:MSVC>:
		/W4
		>)

	target_compile_definitions(${target} PRIVATE
		$<$<BOOL:${MINGW}>:
		__USE_MINGW_ANSI_STDIO=1
		>
		$<$<C_COMPILER_ID:MSVC>:
		_CRT_SECURE_NO_WARNINGS=1
		>)

	if(MATH_LIBRARY)
		target_link_libraries(${target} PUBLIC ${MATH_LIBRARY})
	endif()
endfunction()

add_library(testDriver testDriver.c)
# EXTENSIONS YES заменяет std=c11 на std=gnu11, что выставляет POSIX макросы
set_compiler_settings(testDriver EXTENSIONS YES)

set(TEST_LAB_SOURCES
	testlab-begin.c
	testlab0.c
	testlab1-0.c
	testlab1-1.c
	testlab1-2.c
	testlab2.c
	testlab3-0.c
 	testlab3-3.c
	testlab4.c
	testlab5.c
	testlab6-0.c
	testlab6-1.c
	testlab6-2.c
	testlab6-3.c
	testlab7.c
	testlab8-0.c
	testlab9.c
	testlab10.c
	testlab11.c
	testlab12.c
 	testlabKosorajo.c
)
foreach(source ${TEST_LAB_SOURCES})
	get_filename_component(target ${source} NAME_WE)
	add_executable(${target} ${source})
	set_compiler_settings(${target} EXTENSIONS NO)
	target_link_libraries(${target} PRIVATE testDriver)

	string(REPLACE "testlab" "lab" test ${target})
	add_test(NAME ${test}
		COMMAND "${CMAKE_CURRENT_BINARY_DIR}/${target}"
			"${CMAKE_CURRENT_SOURCE_DIR}/test/${test}$<$<BOOL:${WIN32}>:.exe>")
endforeach()
