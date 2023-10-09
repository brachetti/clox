cmake_minimum_required(VERSION 3.27.0)
project(clox VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

add_executable(clox src/main.c src/chunk.c src/compiler.c src/debug.c src/memory.c src/object.c src/scanner.c src/table.c src/value.c src/vm.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
