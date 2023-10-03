#!/bin/bash

gcc -fdiagnostics-color=always -Wall -g src/main.c src/debug.c src/chunk.c src/memory.c -o target/main
