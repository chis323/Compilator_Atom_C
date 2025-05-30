// lexical_analyzer.h

#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MaxTokenLen 100
#define MaxTokensInList 1000
#define MAX_SYM 1000
#define MAX_ARGS 20

typedef enum {
    T_IDENTIFIER,
    T_BREAK,
    T_CHAR,
    T_DOUBLE,
    T_ELSE,
    T_FOR,
    T_IF,
    T_INT,
    T_RETURN,
    T_STRUCT,
    T_VOID,
    T_WHILE,
    T_CONSTANT_INT,
    T_CONSTANT_REAL,
    T_CONSTANT_CHAR,
    T_CONSTANT_STRING,
    T_COMMA,
    T_SEMICOLON,
    T_LEFT_PAR,
    T_RIGHT_PAR,
    T_LEFT_BRACKET,
    T_RIGHT_BRACKET,
    T_LEFT_ACCOLADE,
    T_RIGHT_ACCOLADE,
    T_ADD,
    T_SUB,
    T_MULT,
    T_DIV,
    T_DOT,
    T_AND,
    T_OR,
    T_NOT,
    T_ASSIGN,
    T_EQUAL,
    T_NOT_EQUAL,
    T_LESS,
    T_LESS_EQUAL,
    T_GREATER,
    T_GREATER_EQUAL,
    T_SPACE,
    T_COMMENT,
    T_LINE_COMMENT,
    T_EOF
} TokenType;

union Value {
    char string_value[MaxTokenLen];
    char char_value;
};

typedef struct {
    TokenType type;
    union Value value;
} Token;


//////////

typedef enum
{
    TB_INT,
    TB_DOUBLE,
    TB_CHAR,
    TB_STRUCT,
    TB_VOID
} TypeBase;

struct Symbol;

typedef struct Type{
    int typeBase;           // TB_*
    struct Symbol *s;        // struct definition for TB_STRUCT
    int nElements;          // >0 array of given size, 0=array without size, <0 non array
} Type;

typedef enum
{
    CLS_VAR,
    CLS_FUNC,
    CLS_EXTFUNC,
    CLS_STRUCT
} Cls;

typedef enum
{
    MEM_GLOBAL,
    MEM_ARG,
    MEM_LOCAL
} Mem;

typedef struct Symbol {
    const char *name;       // a reference to the name stored in a token
    int cls;                // CLS_*
    int mem;                // MEM_*
    Type type;
    int depth;              // 0-global, 1-in function, 2... - nested blocks in function
    union {
        struct Symbol *args;     // pointer to array of arguments (functions)
        struct Symbol *members; // pointer to array of members (structs)
    };
} Symbol;

extern Token token_list[MaxTokensInList];
extern int token_list_index;

extern Symbol symbols[MAX_SYM];
extern int symbolCount;
extern int crtDepth;
extern Symbol* crtFunc;
extern Symbol* crtStruct;

const char *token_type_to_string(TokenType type);
TokenType get_keyword_type(const char *word);
int integer(const char *str);
int real_constant(const char *str);
void add_token(TokenType type, const char *value);
void Token_Formation(const char *formation);
void token_delimiter(char chr);
void token_operators(char curr, char next, FILE *file);
void process_char(char curr, FILE *file, char *formation, int *index, int *is_formation, int *in_string);
Token *tokenize(const char *filename);
Token *get_tokens();
int syntactic_main();
int semantic_main();
void addVar(char name[], Type *t);
Type* getType(TokenType type);
Symbol *addSymbol(Symbol *symbols, int *count,const char *name, int cls);
void printSymbolTable();
#endif
