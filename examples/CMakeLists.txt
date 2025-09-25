cmake_minimum_required(VERSION 3.10)
project(examples C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../lib 
		 ${CMAKE_CURRENT_BINARY_DIR}/argparse)

add_executable(01_person 01_person/main.c)
target_link_libraries(01_person PRIVATE argparse)
target_include_directories(01_person PRIVATE 
		${CMAKE_CURRENT_SOURCE_DIR}/../lib/include)

