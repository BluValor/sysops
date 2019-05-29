cmake_minimum_required(VERSION 3.13)
project(zad1 C)

set(CMAKE_C_STANDARD 11)

add_executable(zad1 main.c exit_func.h exit_func.c image.h image.c filter.h shared_func.h shared_func.c filter.c filter_generator.c positive_filter_generator.c)