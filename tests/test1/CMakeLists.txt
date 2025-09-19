cmake_minimum_required(VERSION 3.31.7)

set (CMAKE_C_STANDARD 23)

project(test1 LANGUAGES C)

add_executable(program_1 program/program_1.c)
add_executable(hello program/program_2.c)
add_executable(world program/program_2.c)
add_executable(ura program/program_2.c)

add_executable(starter ../../src/starter.c)
add_executable(find_lib ../../src/help_programs/find_lib.c)
add_executable(get_libs ../../src/help_programs/get_libs.c)
add_executable(read_config ../../src/help_programs/read_config.c)

add_library(lib_1 SHARED lib/lib_1.c)
add_library(lib_2 SHARED lib/lib_2.c)

set (path ../../../shm_lib/src)
add_library( SharedMemoryLibrary SHARED
	${path}/shm_close.c		${path}/shm_destroy.c
	${path}/shm_getData.c	${path}/shm_rewrite.c
	${path}/shm_simple_open.c	${path}/shm_write.c
)
set_target_properties( SharedMemoryLibrary PROPERTIES
	PREFIX "lib_"
	OUTPUT_NAME "SharedMemoryLibrary"
	SUFFIX ".so"
)

target_link_libraries(program_1 PRIVATE lib_1 lib_2)
