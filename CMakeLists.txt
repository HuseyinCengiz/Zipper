cmake_minimum_required(VERSION 3.12)
project(Zipper C)

set(CMAKE_C_STANDARD 99)

add_executable(Zipper src/main.c src/fields.c include/fields.h)