//> Scanning on Demand compiler-c
#include <stdio.h>
//> Compiling Expressions compiler-include-stdlib
#include <stdlib.h>
//< Compiling Expressions compiler-include-stdlib

#include "common.h"
#include "chunk.h"
#include "compiler.h"
//> Garbage Collection compiler-include-memory
#include "memory.h"
//< Garbage Collection compiler-include-memory
#include "scanner.h"

//> Compiling Expressions include-debug
#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif
//< Compiling Expressions include-debug

//> Compiling Expressions parser
typedef struct
{
    Token current;
    Token previous;
    //> had-error-field
    bool hadError;
    //< had-error-field
    //> panic-mode-field
    bool panicMode;
    //< panic-mode-field
} Parser;

//> precedence

typedef enum
{
    PREC_NONE,
    PREC_ASSIGNMENT, // =
    PREC_OR,         // or
    PREC_AND,        // and
    PREC_EQUALITY,   // == !=
    PREC_COMPARISON, // < > <= >=
    PREC_TERM,       // + -
    PREC_FACTOR,     // * /
    PREC_UNARY,      // ! -
    PREC_CALL,       // . ()
    PREC_PRIMARY
} Precedence;
//< precedence
//> parse-fn-type

//< parse-fn-type
typedef void (*ParseFn)();

//> parse-rule

typedef struct
{
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;
//< parse-rule
//> Local Variables local-struct

Parser parser;
//< Compiling Expressions parser

Chunk *compilingChunk;

//> Calls and Functions current-chunk
static Chunk *currentChunk()
{
    return compilingChunk;
}
//< Calls and Functions current-chunk

//< Compiling Expressions compiling-chunk
//> Compiling Expressions error-at
static void errorAt(Token *token, const char *message)
{
    //> check-panic-mode
    if (parser.panicMode)
        return;
    //< check-panic-mode
    //> set-panic-mode
    parser.panicMode = true;
    //< set-panic-mode
    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TOKEN_EOF)
    {
        fprintf(stderr, " at end");
    }
    else if (token->type == TOKEN_ERROR)
    {
        // Nothing.
    }
    else
    {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser.hadError = true;
}
//< Compiling Expressions error-at
//> Compiling Expressions error
static void error(const char *message)
{
    errorAt(&parser.previous, message);
}
//< Compiling Expressions error
//> Compiling Expressions error-at-current
static void errorAtCurrent(const char *message)
{
    errorAt(&parser.current, message);
}
//< Compiling Expressions error-at-current
//> Compiling Expressions advance

static void advance()
{
    parser.previous = parser.current;

    for (;;)
    {
        parser.current = scanToken();
        if (parser.current.type != TOKEN_ERROR)
            break;

        errorAtCurrent(parser.current.start);
    }
}
//< Compiling Expressions advance

//> Compiling Expressions consume
static void consume(TokenType type, const char *message)
{
    if (parser.current.type == type)
    {
        advance();
        return;
    }

    errorAtCurrent(message);
}
//< Compiling Expressions consume

//> Global Variables check
static bool check(TokenType type) {
  return parser.current.type == type;
}
//< Global Variables check

//> Global Variables match
static bool match(TokenType type)
{
    if (!check(type))
        return false;
    advance();
    return true;
}
//< Global Variables match
//> Compiling Expressions emit-byte
static void emitByte(uint8_t byte)
{
    writeChunk(currentChunk(), byte, parser.previous.line);
}
//< Compiling Expressions emit-byte
//> Compiling Expressions emit-bytes
static void emitBytes(uint8_t byte1, uint8_t byte2)
{
    emitByte(byte1);
    emitByte(byte2);
}
//< Compiling Expressions emit-bytes

//> Compiling Expressions emit-return
static void emitReturn()
{
    //< Methods and Initializers return-this
    emitByte(OP_RETURN);
}
//< Compiling Expressions emit-return

//> Calls and Functions end-compiler
static void endCompiler()
{
    //< Calls and Functions end-compiler
    emitReturn();
#ifdef DEBUG_PRINT_CODE
    if (!parser.hadError) {
        disassembleChunk(currentChunk(), "code");
    }
#endif
    //< Calls and Functions return-function
}
//< Compiling Expressions end-compiler

//> Compiling Expressions forward-declarations

static void expression();
//< Global Variables forward-declarations
static ParseRule *getRule(TokenType type);
static void parsePrecedence(Precedence precedence);

static void expression()
{
    parsePrecedence(PREC_ASSIGNMENT);
}

//> Compiling Expressions make-constant
static uint8_t makeConstant(Value value)
{
    int constant = addConstant(currentChunk(), value);
    if (constant > UINT8_MAX)
    {
        error("Too many constants in one chunk.");
        return 0;
    }

    return (uint8_t)constant;
}
//< Compiling Expressions make-constant

//> Compiling Expressions emit-constant
static void emitConstant(Value value)
{
    emitBytes(OP_CONSTANT, makeConstant(value));
}
//< Compiling Expressions emit-constant

//> Compiling Expressions binary
static void binary()
{
    TokenType operatorType = parser.previous.type;
    ParseRule *rule = getRule(operatorType);
    parsePrecedence((Precedence)(rule->precedence + 1));

    switch (operatorType)
    {
        //< Types of Values comparison-operators
    case TOKEN_PLUS:
        emitByte(OP_ADD);
        break;
    case TOKEN_MINUS:
        emitByte(OP_SUBTRACT);
        break;
    case TOKEN_STAR:
        emitByte(OP_MULTIPLY);
        break;
    case TOKEN_SLASH:
        emitByte(OP_DIVIDE);
        break;
    default:
        return; // unreachable
    }
}
//< Compiling Expressions binary

static void number()
{
    double value = strtod(parser.previous.start, NULL);
    emitConstant(NUMBER_VAL(value));
}

//> Global Variables grouping
static void grouping()
{
    //< Global Variables grouping
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}
//< Compiling Expressions grouping

//> Global Variables unary
static void unary()
{
    //< Global Variables unary
    TokenType operatorType = parser.previous.type;

    // Compile the operand.
    /* Compiling Expressions unary < Compiling Expressions unary-operand
      expression();
    */
    //> unary-operand
    parsePrecedence(PREC_UNARY);
    //< unary-operand

    // Emit the operator instruction.
    switch (operatorType)
    {
        //> Types of Values compile-not
    case TOKEN_BANG:
        emitByte(OP_NOT);
        break;
        //< Types of Values compile-not
    case TOKEN_MINUS:
        emitByte(OP_NEGATE);
        break;
    default:
        return; // Unreachable.
    }
}
//< Compiling Expressions unary

//> Compiling Expressions rules
ParseRule rules[] = {
//< Calls and Functions infix-left-paren
    [TOKEN_LEFT_PAREN]    = {grouping, NULL,   PREC_NONE},
    [TOKEN_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
    [TOKEN_LEFT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_RIGHT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_COMMA] = {NULL, NULL, PREC_NONE},
    [TOKEN_DOT] = {NULL, NULL, PREC_NONE},
    [TOKEN_MINUS] = {unary, binary, PREC_TERM},
    [TOKEN_PLUS] = {NULL, binary, PREC_TERM},
    [TOKEN_SEMICOLON] = {NULL, NULL, PREC_NONE},
    [TOKEN_SLASH] = {NULL, binary, PREC_FACTOR},
    [TOKEN_STAR] = {NULL, binary, PREC_FACTOR},
    [TOKEN_BANG] = {NULL, NULL, PREC_NONE},
    [TOKEN_BANG_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_EQUAL_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_GREATER] = {NULL, NULL, PREC_NONE},
    [TOKEN_GREATER_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_LESS] = {NULL, NULL, PREC_NONE},
    [TOKEN_LESS_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_IDENTIFIER] = {NULL, NULL, PREC_NONE},
    [TOKEN_STRING] = {NULL, NULL, PREC_NONE},
    [TOKEN_NUMBER] = {number, NULL, PREC_NONE},
    [TOKEN_AND] = {NULL, NULL, PREC_NONE},
    [TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
    [TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
    [TOKEN_FALSE] = {NULL, NULL, PREC_NONE},
    [TOKEN_FOR] = {NULL, NULL, PREC_NONE},
    [TOKEN_FUN] = {NULL, NULL, PREC_NONE},
    [TOKEN_IF] = {NULL, NULL, PREC_NONE},
    [TOKEN_NIL] = {NULL, NULL, PREC_NONE},
    [TOKEN_OR] = {NULL, NULL, PREC_NONE},
    [TOKEN_PRINT] = {NULL, NULL, PREC_NONE},
    [TOKEN_RETURN] = {NULL, NULL, PREC_NONE},
    [TOKEN_SUPER] = {NULL, NULL, PREC_NONE},
    [TOKEN_THIS] = {NULL, NULL, PREC_NONE},
    [TOKEN_TRUE] = {NULL, NULL, PREC_NONE},
    [TOKEN_VAR] = {NULL, NULL, PREC_NONE},
    [TOKEN_WHILE] = {NULL, NULL, PREC_NONE},
    [TOKEN_ERROR] = {NULL, NULL, PREC_NONE},
    [TOKEN_EOF] = {NULL, NULL, PREC_NONE},
};
//< Compiling Expressions rules

//> Compiling Expressions parse-precedence
static void parsePrecedence(Precedence precedence) {
    advance();
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;
    if (prefixRule == NULL) {
        error("Expect expression.");
        return;
    }

    prefixRule();

    while (precedence <= getRule(parser.current.type)->precedence) {
        advance();
        ParseFn infixRule = getRule(parser.previous.type)->infix;
        infixRule();
    }
}
//< Compiling Expressions parse-precedence

//> Compiling Expressions get-rule
static ParseRule* getRule(TokenType type) {
  return &rules[type];
}
//< Compiling Expressions get-rule

bool compile(const char *source, Chunk *chunk)
{
    initScanner(source);
    compilingChunk = chunk;

    parser.panicMode = false;
    parser.hadError = false;

    advance();
    expression();
    consume(TOKEN_EOF, "Expect end of expression.");
    endCompiler();

    return !parser.hadError;
}
