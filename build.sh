#!/bin/bash

gcc -fdiagnostics-color=always -Wall \
    -g src/main.c src/chunk.c src/compiler.c src/debug.c src/memory.c src/scanner.c src/value.c src/vm.c \
    -o target/main
