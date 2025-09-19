cmake_minimum_required(VERSION 3.31.7)
include (base-kit.cmake)

message(DEBUG "${magenta}Starting compiling${blue} 'imagine a very cool project name'${yellow} library${rc}${gray}...${rc}")

project(starter
	VERSION ${ver}
	LANGUAGES ${supported_languages})


add_executable(${PROJECT_NAME} src/starter.c)
