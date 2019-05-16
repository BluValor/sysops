cmake_minimum_required(VERSION 3.13)
project(zad1 C)

set(CMAKE_C_STANDARD 99)

add_executable(zad1 ipc_func.h ipc_systemv.c exit_func.c package.h package.c shared_func.c cbelt.h cbelt.c truck.c worker.c boss.c shared_func.h exit_func.h ipc_posix.c)