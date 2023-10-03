cmake_minimum_required(VERSION 3.27.0)
project(clox VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

add_executable(clox src/main.c src/chunk.c src/memory.c src/debug.c src/value.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
