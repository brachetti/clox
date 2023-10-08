//> Scanning on Demand compiler-h
#ifndef clox_compiler_h
#define clox_compiler_h

#include <stdlib.h>

//> Compiling Expressions compile-h
#include "chunk.h"
//< Compiling Expressions compile-h
#include "object.h"
#include "vm.h"

//> Calls and Functions compile-h

bool compile(const char* source, Chunk* chunk);

//< Calls and Functions compile-h

#endif