//> Strings object-h
#ifndef clox_object_h
#define clox_object_h

#include "common.h"
//> Calls and Functions object-include-chunk
//< Classes and Instances object-include-table
#include "value.h"
//> obj-type-macro

#define OBJ_TYPE(value) (AS_OBJ(value)->type)
//< obj-type-macro
//> is-string

#define IS_STRING(value) isObjType(value, OBJ_STRING)
//< is-string
//> as-string

#define AS_STRING(value) ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString*)AS_OBJ(value))->chars)
//< as-string
//> obj-type

typedef enum { OBJ_STRING } ObjType;
//< obj-type

struct Obj {
  ObjType     type;
  struct Obj* next;
};

struct ObjString {
  struct Obj obj;
  int        length;
  char*      chars;
  uint32_t   hash;
};
//< obj-string

ObjString* takeString(char* chars, int length);
ObjString* copyString(const char* chars, int length);
void       printObject(Value value);

//< copy-string-h
//> is-obj-type
static inline bool isObjType(Value value, ObjType type) {
  return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

//< is-obj-type
#endif
