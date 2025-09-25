cmake_minimum_required(VERSION 3.10)
project(argparse C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(include)

add_library(argparse STATIC
    source/argparse.c 
)
