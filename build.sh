#!/bin/bash

gcc -fdiagnostics-color=always -Wall \
    -g src/main.c src/chunk.c src/compiler.c src/debug.c \
        src/memory.c src/scanner.c src/table.c \
        src/value.c src/vm.c src/object.c \
    -o target/main
