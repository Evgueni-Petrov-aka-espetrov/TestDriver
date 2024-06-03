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

set(TEST_LAB_BEGIN "testlab-begin.c")
set(TEST_LAB_0 "testlab0.c")
set(TEST_LAB_1-0 "testlab1-0.c")
set(TEST_LAB_1-1 "testlab1-1.c")
set(TEST_LAB_1-2 "testlab1-2.c")
set(TEST_LAB_2 "testlab2.c")
set(TEST_LAB_3-0 "testlab3-0.c")
set(TEST_LAB_4 "testlab4.c")
set(TEST_LAB_5 "testlab5.c")
set(TEST_LAB_6-0 "testlab6-0.c")
set(TEST_LAB_6-1 "testlab6-1.c")
set(TEST_LAB_6-2 "testlab6-2.c")
set(TEST_LAB_6-3 "testlab6-3.c")
set(TEST_LAB_7 "testlab7.c")
set(TEST_LAB_8-0 "testlab8-0.c" "testlab8-base.c" "testlab8-base.h")
set(TEST_LAB_8-1 "testlab8-1.c" "testlab8-base.c" "testlab8-base.h")
set(TEST_LAB_9 "testlab9.c")
set(TEST_LAB_10 "testlab10.c")
set(TEST_LAB_11 "testlab11.c")
set(TEST_LAB_12 "testlab12.c")
set(TEST_LAB_KOSORAJO "testlabKosorajo.c")

set(TEST_LAB_SOURCES
	TEST_LAB_BEGIN
	TEST_LAB_0
	TEST_LAB_1-0
	TEST_LAB_1-1
	TEST_LAB_1-2
	TEST_LAB_2
	TEST_LAB_3-0
	TEST_LAB_4
	TEST_LAB_5
	TEST_LAB_6-0
	TEST_LAB_6-1
	TEST_LAB_6-2
	TEST_LAB_6-3
	TEST_LAB_7
	TEST_LAB_8-0
	TEST_LAB_8-1
	TEST_LAB_9
	TEST_LAB_10
	TEST_LAB_11
	TEST_LAB_12
 	TEST_LAB_KOSORAJO
)
foreach(source ${TEST_LAB_SOURCES})
	list(GET ${source} 0 first)
	get_filename_component(target ${first} NAME_WE)
	add_executable(${target} ${${source}})
	set_compiler_settings(${target} EXTENSIONS NO)
	target_link_libraries(${target} PRIVATE testDriver)

	string(REPLACE "testlab" "lab" test ${target})
	add_test(NAME ${test}
		COMMAND "${CMAKE_CURRENT_BINARY_DIR}/${target}"
			"${CMAKE_CURRENT_SOURCE_DIR}/test/${test}$<$<BOOL:${WIN32}>:.exe>")
endforeach()
